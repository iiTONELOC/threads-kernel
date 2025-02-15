#define _CRT_SECURE_NO_WARNINGS

#include "SchedulerHelpers.h"

int nextPid = 1;
int debugFlag = 1;
int isBooting = 1;
int processCount = 0;

ReadyList readyList = {0};
Process *runningProcess = NULL;
ProcessTable processTable = {0};
MasterList linkedListMaster = {0};
interrupt_handler_t *interruptHandlers = NULL; // interrupt handlers from THREADS API

int cpu_time();
void time_slice();
void dispatcher();

void enforceKernelMode();
static int launch(void *);
static int watchdog(void *);
static void check_deadlock();

static inline void enableInterrupts();
static inline void disableInterrupts();
static void DebugConsole(char *format, ...);
void clockInterruptHandler(void *device, uint8_t command, uint32_t status);

/* DO NOT REMOVE */
int check_io_scheduler();
check_io_function check_io;
extern int SchedulerEntryPoint(void *pArgs);
/*************************************************************************
   bootstrap()

   Purpose - This is the first function called by THREADS on startup.

             The function must setup the OS scheduler and primitive
             functionality and then spawn the first two processes.

             The first two process are the watchdog process
             and the startup process SchedulerEntryPoint.

             The statup process is used to initialize additional layers
             of the OS.  It is also used for testing the scheduler
             functions.

   Parameters - Arguments *pArgs - these arguments are unused at this time.

   Returns - The function does not return!

   Side Effects - The effects of this function is the launching of the kernel.

 *************************************************************************/
int bootstrap(void *pArgs)
{
    int result; /* value returned by call to spawn() */

    /* set this to the scheduler version of this function.*/
    check_io = check_io_scheduler;

    /* Initialize the ready list */
    SchedulerInitReadyList();

    /* Initialize the clock interrupt handler */
    interruptHandlers = get_interrupt_handlers();
    interruptHandlers[THREADS_TIMER_INTERRUPT] = (interrupt_handler_t)&clockInterruptHandler;

    /* startup a watchdog process */
    result = k_spawn("watchdog", watchdog, NULL, THREADS_MIN_STACK_SIZE, LOWEST_PRIORITY);
    if (result < 0)
    {
        console_output(debugFlag, "Scheduler(): spawn for watchdog returned an error (%d), stopping...\n", result);
        stop(1);
    }

    /* Set booting to false so the dispatcher selects the next ready process*/
    isBooting = 0;

    /* start the test process, which is the main for each test program.  */
    result = k_spawn("Scheduler", SchedulerEntryPoint, NULL, 2 * THREADS_MIN_STACK_SIZE, HIGHEST_PRIORITY);
    if (result < 0)
    {
        console_output(debugFlag, "Scheduler(): spawn for SchedulerEntryPoint returned an error (%d), stopping...\n", result);
        stop(1);
    }

    /* Initialized and ready to go!! */

    /* This should never return since we are not a real process. */

    stop(-3);
    return 0;
}

/*************************************************************************
   k_spawn()

   Purpose - spawns a new process.

             Finds an empty entry in the process table and initializes
             information of the process.  Updates information in the
             parent process to reflect this child process creation.

   Parameters - the process's entry point function, the stack size, and
                the process's priority.

   Returns - The Process ID (pid) of the new child process
             The function must return if the process cannot be created.

************************************************************************ */
int k_spawn(char *name, int (*entryPoint)(void *), void *arg, int stacksize, int priority)
{
    enforceKernelMode();
    disableInterrupts();

    int proc_slot;                 /* slot in the process table */
    Process *pNewProc;             /* pointer to the new process */
    NewProcessArgs newProcessArgs; /* arguments for the new process */

    /* Validate the parameters */
    if (ValidateKSpawnParams(name, entryPoint, arg, stacksize, priority, debugFlag) != 0)
    {
        return -1;
    }

    /* Find an empty slot in the process table */
    proc_slot = GetEmptyControlBlockIndex(&processTable[0]);
    pNewProc = &processTable[proc_slot];

    /* Create the arg struct*/
    newProcessArgs = (NewProcessArgs){
        .arg = arg,
        .name = name,
        .pid = nextPid,
        .priority = priority,
        .procSlot = proc_slot,
        .stacksize = stacksize,
        .entryPoint = entryPoint,
        .pNewProcess = &processTable[proc_slot]};

    /* Create the new process */
    SchedulerCreateNewProcess(&newProcessArgs);

    /* Initialize the process' context */
    pNewProc->context = context_initialize(launch, stacksize, arg);

    /* Call the  dispatcher*/
    dispatcher();
    return pNewProc->pid;

} /* spawn */

/**************************************************************************
   Name - launch

   Purpose - Utility function that makes sure the environment is ready,
             such as enabling interrupts, for the new process.

   Parameters - none

   Returns - nothing
*************************************************************************/
static int launch(void *args)
{
    enforceKernelMode();
    int result = 0;

    /* Enable interrupts */
    enableInterrupts();
    // set the start time for the process
    runningProcess->startTime = system_clock();
    /* Call the function passed to spawn and capture its return value */
    result = runningProcess->entryPoint(runningProcess->startArgs);
    /* Stop the process gracefully */
    k_exit(result);
    return 0;
}

/**************************************************************************
   Name - k_wait

   Purpose - Wait for a child process to quit.  Return right away if
             a child has already quit.

   Parameters - Output parameter for the child's exit code.

   Returns - the pid of the quitting child, or
        -4 if the process has no children
        -5 if the process was signaled in the join

************************************************************************ */
int k_wait(int *code)
{
    enforceKernelMode();
    disableInterrupts();

    int result = 0;
    Process *pChildProcess = NULL;

    // look for children that exited before the parent could wait for them
    result = HandleZombieChildren(runningProcess, code);
    if (result != -1500)
    {
        // zombie children were cleaned up, return the result
        enableInterrupts();
        return result;
    }

    // if there are no child processes to wait for return -1
    if (runningProcess != NULL &&
        linkedListMaster[runningProcess->processTableIndex][GetProcessListIndex(PROCESS_CHILDREN_LIST)].count == 0)
    {
        enableInterrupts();
        return -1;
    }

    // block ourselves on a blocked wait
    runningProcess->status = STATUS_BLOCKED_ON_WAIT;
    runningProcess = NULL;
    dispatcher();

    // ----- AFTER PROCESS WAS AWAKENED BY AN EXITING CHILD -----
    disableInterrupts();

    // get the exit code of the child process
    result = HandleExitingChildren(runningProcess, code);
    // if result is -1500, no exiting children were found
    result = result == -1500 ? 0 : result;

    // call the  dispatcher to get the next process
    dispatcher();

    // return the result or -5 if the process was signaled
    return runningProcess->signal == SIG_TERM ? -5 : result;
}

/**************************************************************************
   Name - k_exit

   Purpose - Exits a process and coordinates with the parent for cleanup
             and return of the exit code.

   Parameters - the code to return to the grieving parent

   Returns - nothing

*************************************************************************/
void k_exit(int code)
{
    enforceKernelMode();
    disableInterrupts();

    // check if the process has children
    if (linkedListMaster[runningProcess->processTableIndex][GetProcessListIndex(PROCESS_CHILDREN_LIST)].count > 0)
    {
        console_output(debugFlag, "quit(): Process with active children attempting to quit\n");
        stop(1);
    }

    // set the exit code of the running process
    // if it was signaled, set the exit code to -5
    runningProcess->exitCode = runningProcess->signal == SIG_TERM ? -5 : code;

    /* Set the status of the process to quit */
    runningProcess->status = STATUS_QUIT;

    /*
     If the process has a parent, add it to the parent's exiting children list
     and unblock the parent process
    */
    if (runningProcess->pParent != NULL)
    {
        // remove the process from the parent's children list
        RemoveProcessFromList(&linkedListMaster[runningProcess->pParent->processTableIndex][GetProcessListIndex(PROCESS_CHILDREN_LIST)],
                              runningProcess);

        // if the parent is blocked on a wait, unblock it
        if (runningProcess->pParent->status == STATUS_BLOCKED_ON_WAIT)
        {
            // add the process to the parent's exiting children list
            AddProcessToList(runningProcess,
                             &linkedListMaster[runningProcess->pParent->processTableIndex][GetProcessListIndex(PROCESS_EXITING_CHILDREN_LIST)]);
            // unblock the parent process
            unblock(runningProcess->pParent->pid);
        }
        else
        {
            // parent isn't blocked so move this process to the zombie list
            AddProcessToList(runningProcess,
                             &linkedListMaster[runningProcess->pParent->processTableIndex][GetProcessListIndex(PROCESS_ZOMBIE_CHILDREN_LIST)]);
        }
    }
    else
    {
        // if the process has no parent
        // stop the context
        context_stop(runningProcess->context);
        // reset the child process in the process table
        memset(runningProcess, 0, sizeof(Process));
        // decrement the process count
        processCount--;
    }

    /* Call the dispatcher */
    runningProcess = NULL;
    dispatcher();
}

/**************************************************************************
   Name - k_kill

   Purpose - Signals a process with the specified signal

   Parameters - Signal to send

   Returns -
*************************************************************************/
int k_kill(int pid, int signal)
{
    enforceKernelMode();
    disableInterrupts();

    int index = SchedulerPidToIndex(pid);     // get the index of the process in the process table
    Process *pProcess = &processTable[index]; // get the process from the process table

    // if the process is not found OR the signal is not equal to SIG_TERM, stop(1)
    if (pProcess == NULL || signal != SIG_TERM)
    {
        stop(1);
    }

    // if the process has already been signaled, return 1
    if (pProcess->signal == SIG_TERM)
    {
        enableInterrupts();
        return 1;
    }

    // set the signal of the process
    pProcess->signal = signal;
    dispatcher();
    return 0;
}

/**************************************************************************
   Name - k_getpid
*************************************************************************/
int k_getpid()
{
    return runningProcess != NULL ? runningProcess->pid : -1;
}

/**************************************************************************
   Name - k_join
***************************************************************************/
int k_join(int pid, int *pChildExitCode)
{
    return 0;
}

/**************************************************************************
   Name - unblock
*************************************************************************/
int unblock(int pid)
{
    enforceKernelMode();
    disableInterrupts();
    int index = SchedulerPidToIndex(pid);
    Process *pProcess = &processTable[index];
    enum ProcessStatus status = pProcess->status;

    // checks for a valid process and if it is blocked
    if (pProcess != NULL &&
        (status > STATUS_RUNNING && status < STATUS_QUIT))
    {
        pProcess->status = STATUS_READY;
        AddProcessToList(pProcess, &readyList[pProcess->priority]);
    }
    else
    {
        // process  does not exist or is not blocked
        enableInterrupts();
        return -1;
    }

    dispatcher();

    return 0;
}

/*************************************************************************
   Name - block
*************************************************************************/
int block(int newStatus)
{

    return 0;
}

/*************************************************************************
   Name - signaled
*************************************************************************/
int signaled()
{
    enforceKernelMode();
    return (runningProcess && runningProcess->signal) ? 1 : 0;
}
/*************************************************************************
   Name - readtime
*************************************************************************/
int read_time()
{
    enforceKernelMode();
    return cpu_time();
}

/*************************************************************************
   Name - cpu_time
*************************************************************************/
int cpu_time()
{
    enforceKernelMode();
    return runningProcess ? runningProcess->cpuTime : 0;
}

/*************************************************************************
   Name - readClock
*************************************************************************/
DWORD read_clock()
{
    enforceKernelMode();
    return system_clock();
}

/**************************************************************************
   Name - get_start_time
*************************************************************************/
int get_start_time(void)
{
    enforceKernelMode();
    return runningProcess->startTime;
}

/**************************************************************************
   Name - display_process_table
*************************************************************************/
void display_process_table()
{
    enforceKernelMode();
    PrintProcessTable(processTable, MAX_PROCESSES);
}

/**************************************************************************
   Name - dispatcher

   Purpose - This is where context changes to the next process to run.

   Parameters - none

   Returns - nothing

*************************************************************************/
void dispatcher()
{
    enforceKernelMode();

    Process *pNextProcess = NULL;

    if (isBooting)
    {
        return;
    }

    // if there is a running process, check to see if its quantum has expired

    pNextProcess = SchedulerGetNextProcess();

    // if a process is returned, handle the context switch
    if (pNextProcess != NULL && pNextProcess != runningProcess)
    {
        SchedulerHandleContextSwitch(pNextProcess);
    }

    return;
}

/**************************************************************************
   Name - watchdog

   Purpose - The watchdoog keeps the system going when all other
         processes are blocked.  It can be used to detect when the system
         is shutting down as well as when a deadlock condition arises.

   Parameters - none

   Returns - nothing
   *************************************************************************/

static int watchdog(void *args)
{
    enforceKernelMode();

    Process *pNextReadyProcess;

    if (isBooting)
    {
        // We are still booting up, so we need to wait for the system to be ready
        return 0;
    }

    pNextReadyProcess = SchedulerGetNextProcess();

    if (pNextReadyProcess == NULL)
    {
        // If there are no ready processes, we could be in a deadlock
        check_deadlock();
        console_output(debugFlag, "watchdog(): PROCESSES STILL EXIST\n");
        stop(-3);
    }
    else
    {
        dispatcher();
    }

    return 0;
}

/* check to determine if deadlock has occurred... */
static void check_deadlock()
{
    enforceKernelMode();
    int i = 0;
    Process *pCurrentProcess = NULL;

    while (1)
    {
        disableInterrupts();
        // check to see if there are more than 2 processes left in the system
        if (processCount > 2)
        {
            console_output(debugFlag, "check_deadlock(): PROCESSES STILL EXIST\n");
            enableInterrupts();
        }
        else
        {
            // no processes are running, and there are none left to run
            break;
        }
    }

    // if we get here, we broke out so all process have been completed
    console_output(debugFlag, "All processes completed.\n");

    // stop the system
    stop(0);
}

/*
 * Disables the interrupts.
 */
static inline void disableInterrupts()
{
    enforceKernelMode();

    /* We ARE in kernel mode */
    set_psr(get_psr() & ~PSR_INTERRUPTS);
}

/*
 * Enables the interrupts.
 */
static inline void enableInterrupts()
{
    enforceKernelMode();

    set_psr(get_psr() | PSR_INTERRUPTS);
}

/**************************************************************************
   Name - DebugConsole
   Purpose - Prints  the message to the console_output if in debug mode
   Parameters - format string and va args
   Returns - nothing
   Side Effects -
*************************************************************************/
static void DebugConsole(char *format, ...)
{
    char buffer[2048];
    va_list argptr;

    if (debugFlag)
    {
        va_start(argptr, format);
        vsprintf(buffer, format, argptr);
        console_output(TRUE, buffer);
        va_end(argptr);
    }
}

/* there is no I/O yet, so return false. */
int check_io_scheduler()
{
    enforceKernelMode();
    return false;
}
void time_slice()
{
    enforceKernelMode();
    static int elapsed = 0;           // time elapsed since last context switch
    static int lastTime = 0;          // last time the clock interrupt was called
    int currentTime = system_clock(); // current time in μs but

    // if there is a running process and it doesn't have a start time, set it
    if (runningProcess != NULL && runningProcess->startTime == 0)
    {
        // needs to be in μs
        runningProcess->startTime = currentTime;
    }

    // calculate the time elapsed since the last context switch
    if (lastTime != 0)
    {
        // should be in μs
        elapsed += (currentTime - lastTime);
    }

    // update the last time the clock interrupt was called
    lastTime = currentTime;

    // if there is a running process, update its elapsed and cpu time
    if (runningProcess != NULL)
    {
        runningProcess->elapsedTime += elapsed;
        runningProcess->cpuTime += (elapsed / NUM_MILLI_SEC_IN_MICRO_SEC);
    }

    // check if the elapsed time is greater than the quantum for the running process
    if (runningProcess != NULL && runningProcess->elapsedTime >= runningProcess->quantum)
    {
        elapsed = 0;
        runningProcess->elapsedTime = 0;
        dispatcher();
    }
    else
    {
        elapsed = 0;
    }
}

/**
 * @brief Clock interrupt handler
 *
 * @param device A pointer to the device id
 * @param command The command to execute
 * @param status The status of the device
 *
 * @note This function is called when the clock interrupt is triggered, this function
 * should not be called directly.
 */
void clockInterruptHandler(void *device, uint8_t command, uint32_t status)
{
    time_slice();
}

/**
 * @brief Enforces kernel mode
 *
 * This function checks to see if the processor is in kernel mode,
 * if it isn't the kernel will halt with a status of 1.
 */
void enforceKernelMode()
{

    if ((get_psr() & PSR_KERNEL_MODE) == 0)
    {
        console_output(debugFlag, "Kernel mode expected, but function called in user mode.\n");
        stop(1);
    }
}
