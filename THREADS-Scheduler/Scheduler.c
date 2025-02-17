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
        linkedListMaster[runningProcess->processTableIndex][PLI(PROCESS_CHILDREN_LIST)].count == 0)
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
    enableInterrupts();
  /*  dispatcher();*/

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
    int tableIndex = runningProcess->processTableIndex;

    // check if the process has children
    if (linkedListMaster[tableIndex][PLI(PROCESS_CHILDREN_LIST)].count > 0)
    {
        console_output(debugFlag, "quit(): Process with active children attempting to quit\n");
        stop(1);
    }

    // set the exit code of the running process
    // if it was signaled, set the exit code to -5
    runningProcess->exitCode = runningProcess->signal == SIG_TERM ? -5 : code;

    /* Set the status of the process to quit */
    runningProcess->status = STATUS_QUIT;

    /* If the process has processes waiting to join it */
    if (linkedListMaster[tableIndex][PLI(PROCESS_JOINING_PROCESSES_LIST)].count > 0)
    {
        WakeUpJoiners();
    }

    /*
     If the process has a parent, notify the parent of the exit
    */
    if (runningProcess->pParent != NULL)
    {
        /* A process' parent will wake up the child */
        ChildNotifyParentOfExit();
    }
    else
    {
        /* Otherwise, clean up the process */
        SchedulerCleanUpProcess(runningProcess);
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
    enforceKernelMode();
    disableInterrupts();

    int prevCount = 0;
    int index = SchedulerPidToIndex(pid);           // get the index of the process in the process table
    Process *pProcessToJoin = &processTable[index]; // get the process from the process table

    // if the process is not found
    if (pProcessToJoin == NULL)
    {
        console_output(debugFlag, "join: attempting to join a process that does not exist.\n");
        stop(1);
    }

    // ensure the process is not trying to join itself
    if (pProcessToJoin->pid == runningProcess->pid)
    {
        console_output(debugFlag, "join: process attempting to join itself.\n");
        stop(1);
    }

    // ensure the process is not trying to join its parent
    if (pid == runningProcess->pParent->pid)
    {
        console_output(debugFlag, "join: process attempting to join its parent.\n");
        stop(2);
    }

    prevCount = linkedListMaster[index][PLI(PROCESS_JOINING_PROCESSES_LIST)].count;
    // add the process to the joining list on the process to join
    AddProcessToList(runningProcess,
                     &linkedListMaster[index][PLI(PROCESS_JOINING_PROCESSES_LIST)]);

    // set our status to blocked on join
    runningProcess->status = STATUS_BLOCKED_ON_JOIN;

    // verify we were added to the list
    if (prevCount == linkedListMaster[index][PLI(PROCESS_JOINING_PROCESSES_LIST)].count)
    {
        return -1;
    }
    runningProcess = NULL;
    // call the dispatcher
    dispatcher();

    // ----- AFTER PROCESS WAS AWAKENED BY THE PROCESS IT WANTS TO JOIN -----

    disableInterrupts();

    // get the exit code of the process
    *pChildExitCode = runningProcess->joinStatus;
    runningProcess->joinStatus = -99;

    // call the dispatcher
    dispatcher();

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

    /*  enableInterrupts();*/
    dispatcher();

    return 0;
}

/*************************************************************************
   Name - block
*************************************************************************/
int block(int newStatus)
{
    enforceKernelMode();

    /* if the new status is between 0 and 10 */
    if (newStatus <= (NUM_PROCESS_STATES - 1))
    {
        console_output(debugFlag, "block: function called with a reserved status value.\n");
        stop(1);
    }

    /* Change the status */
    disableInterrupts();
    runningProcess->status = newStatus;

    /* Call the dispatcher */
    dispatcher();

    return runningProcess->signal == SIG_TERM ? -5 : 0;
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
    unsigned int curentTimeSlice = 0;

    if (isBooting)
    {
        return;
    }

    curentTimeSlice = SchedulerCalculateTimeSlice();

    if (curentTimeSlice == 0)
    {
        disableInterrupts();
        // The current time has expired - grab the next process
        pNextProcess = SchedulerGetNextProcess();

        // if the next process is NULL, we could be in a deadlock call watchdog
        if (pNextProcess == NULL)
        {
             watchdog(NULL);
            return;
        }

        if (runningProcess != NULL && runningProcess->pid == pNextProcess->pid)
        {
            enableInterrupts();
            return;
        }

        return SchedulerHandleContextSwitch(pNextProcess);
    }
    else
    {
        disableInterrupts();
        /* Current Quantum has not expired, check for preemtion */
        // TO DO: check the priority level of the next process on the ready list
        pNextProcess = SchedulerGetNextProcess();

        if (pNextProcess != NULL && pNextProcess->priority > runningProcess->priority)
        {
            // preempt the current process and let the higher priority proccess run
            return SchedulerHandleContextSwitch(pNextProcess);
        }
        else if (pNextProcess != NULL && pNextProcess->priority <= runningProcess->priority)
        {
            // place the pNextProcess back on the ready list
            PushProcessToList(&readyList[pNextProcess->priority], pNextProcess);
        }
        enableInterrupts();

        return;
    }

    // If the current time slice is 0, there is no more time remaining

    // disableInterrupts();

    // pNextProcess = SchedulerGetNextProcess();

    // if (pNextProcess == NULL && runningProcess == NULL)
    // {
    //     enableInterrupts();
    //     return;
    // }

    // if (runningProcess == NULL)
    // {
    //     return SchedulerHandleContextSwitch(pNextProcess);
    // }

    // // get the current time slice
    // curentTimeSlice = SchedulerCalculateTimeSlice();

    // /* if there is a running process and its time slice is not up,
    //  and the next process has a lower priority, return */
    // if (runningProcess != NULL &&
    //     pNextProcess != NULL &&
    //     curentTimeSlice > 0 &&
    //     pNextProcess->priority < runningProcess->priority)
    // {
    //     /*put the pNextProcess back to the front of the ready list - we are not using it*/
    //     if (pNextProcess != NULL)
    //     {
    //         pNextProcess->status = STATUS_READY;
    //         PushProcessToList(&readyList[pNextProcess->priority], pNextProcess);
    //     }

    //     enableInterrupts();
    //     return;
    // }

    // /* if there is no running process
    //    or the time slice is up
    //    or the next process is not the same as the running process
    //    or the next process has a higher priority */
    // if ((pNextProcess != NULL && pNextProcess->priority > runningProcess->priority) ||
    //     (pNextProcess != NULL && curentTimeSlice == 0) ||
    //     (pNextProcess != NULL && pNextProcess->pid != runningProcess->pid))
    // {
    //     return SchedulerHandleContextSwitch(pNextProcess);
    // }
    // else
    // {
    //     enableInterrupts();
    // }
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

    disableInterrupts();
    pNextReadyProcess = SchedulerGetNextProcess();

    if (pNextReadyProcess == NULL)
    {
        // If there are no ready processes, we could be in a deadlock
        check_deadlock();
        console_output(debugFlag, "watchdog(): CHECKED DEADLOCK - PROCESSES STILL EXIST\n");
        stop(1);
    }
    else
    {
        // put the process back in the ready list
        PushProcessToList(&readyList[pNextReadyProcess->priority], pNextReadyProcess);
    }
    enableInterrupts();
    return 0;
}

/* check to determine if deadlock has occurred... */
static void check_deadlock()
{
    enforceKernelMode();
    int i = 0;
    Process *pCurrentProcess = NULL;

    disableInterrupts();
    while (1)
    {

        // check to see if there are more than 2 processes left in the system
        if (processCount > 2)
        {
            // process exist, enable interrupts and wait
            // for the next process to run, or for the table to be empty
            console_output(debugFlag, "check_deadlock(): PROCESSES STILL EXIST\n");

            // look in the process table for aprocess that is blocked on a wait
            for (i = 0; i < MAX_PROCESSES; i++)
            {
                pCurrentProcess = &processTable[i];
                if (pCurrentProcess->status == STATUS_BLOCKED_ON_WAIT)
                {
                    // unblock the process
                    pCurrentProcess->status = STATUS_READY;
                    AddProcessToList(pCurrentProcess, &readyList[pCurrentProcess->priority]);
                    dispatcher();
                }
            }

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

/**
 * @brief Looks at the time slice for the current process and determines if it should be preempted
 */
void time_slice()
{
    enforceKernelMode();

    // Calculates the remaining time slice for the current process
    // If the time slice is 0, the process should be preempted
    if (SchedulerCalculateTimeSlice() == 0)
    {
        dispatcher();
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
