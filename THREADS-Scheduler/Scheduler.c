
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerConstants.h"
#include "Scheduler.h"
#include "Processes.h"

int nextPid = 1;
int debugFlag = 1;

Process *runningProcess = NULL;                          // tracks running process
Process processTable[MAX_PROCESSES];                     // storage table for all processes
interrupt_handler_t *interruptHandlers = NULL;           // interrupt handlers
Process processStatusList[4] = {NULL, NULL, NULL, NULL}; // process state list for ready, running, blocked, quit

static int watchdog(char *);
static inline void disableInterrupts();

static inline void enableInterrupts();
void dispatcher();
static int launch(void *);
static void check_deadlock();
static void DebugConsole(char *format, ...);
void clockInterruptHandler(void *device, uint8_t command, uint32_t status);

/* DO NOT REMOVE */
extern int SchedulerEntryPoint(void *pArgs);
int check_io_scheduler();

check_io_function check_io;

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

    /* Initialize the process table. */
    initializeProcessTable(&processTable);

    /* Initialize the Ready list, etc. */
    // initializeProcessList(&processStatusList);

    /* Initialize the clock interrupt handler */
    interruptHandlers = get_interrupt_handlers();
    interruptHandlers[THREADS_TIMER_INTERRUPT] = &clockInterruptHandler;

    /* startup a watchdog process */
    result = k_spawn("watchdog", watchdog, NULL, THREADS_MIN_STACK_SIZE, LOWEST_PRIORITY);

    if (result < 0)
    {
        console_output(debugFlag, "Scheduler(): spawn for watchdog returned an error (%d), stopping...\n", result);
        stop(1);
    }

    /* start the test process, which is the main for each test program.  */
    result = k_spawn("Scheduler", SchedulerEntryPoint, NULL, 2 * THREADS_MIN_STACK_SIZE, HIGHEST_PRIORITY);
    if (result < 0)
    {
        console_output(debugFlag, "Scheduler(): spawn for SchedulerEntryPoint returned an error (%d), stopping...\n", result);
        stop(1);
    }

    // start the first process
    dispatcher();

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
    disableInterrupts();
    int proc_slot;
    struct _process *pNewProc;

    DebugConsole("spawn(): creating process %s\n", name);

    /*Validate all of the parameters, starting with the name. */
    if (name == NULL)
    {
        console_output(debugFlag, "spawn(): Name value is NULL.\n");
        return -1;
    }
    if (strlen(name) >= (MAXNAME - 1))
    {
        console_output(debugFlag, "spawn(): Process name is too long.  Halting...\n");
        stop(1);
    }

    // if (arg == NULL)
    // {
    //     console_output(debugFlag, "spawn(): arg is NULL.\n");
    //     return -1;
    // }

    if (!arg == NULL && strlen(arg) >= (MAXARG - 1))
    {
        console_output(debugFlag, "spawn(): Process arg is too long.  Halting...\n");
        stop(1);
    }

    if (entryPoint == NULL)
    {
        console_output(debugFlag, "spawn(): entryPoint is NULL.\n");
        return -1;
    }

    if (stacksize < THREADS_MIN_STACK_SIZE)
    {
        console_output(debugFlag, "spawn(): Stack size is too small.\n");
        return -2;
    }

    if (priority < LOWEST_PRIORITY || priority > HIGHEST_PRIORITY)
    {
        console_output(debugFlag, "spawn(): Priority is out of range.\n");
        return -3;
    }

    /* Find an empty slot in the process table */
    // provided
    // proc_slot = 1;  // just use 1 for now!
    proc_slot = getEmptyControlBlockIndex(&processTable);
    if (proc_slot == -1)
    {
        console_output(debugFlag, "Process Table is full!.\n");
        return -4;
    }

    /* Get a pointer to the new process control block */
    pNewProc = &processTable[proc_slot];

    /* Setup the entry in the process table. */
    configureProcessForTable(pNewProc, nextPid, proc_slot, name,
                             entryPoint, arg, stacksize, priority);
    nextPid++; // increment the next pid

    /* If there is a parent process,add this to its list of children. */
    if (runningProcess != NULL)
    {
        addChildProcess(pNewProc, &runningProcess);
    }

    // print some information from the processTable
    console_output(debugFlag, "Process %s has been created\n", processTable[proc_slot].name);

    /* Initialize context for this process, but use launch function pointer for
     * the initial value of the process's program counter (PC)
     */
    pNewProc->context = context_initialize(launch, stacksize, arg);

    /* Add the process to the ready list. */
    insertIntoProcessTable(pNewProc, &processTable, &runningProcess, proc_slot);
    insertIntoProcessList(pNewProc, &processStatusList, READY, priority);

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
    int result;

    DebugConsole("launch(): started: %s\n", runningProcess->name);
    // set the start time
    runningProcess->startTime = read_clock();

    /* Enable interrupts */
    enableInterrupts();

    /* Call the function passed to spawn and capture its return value */

    result = runningProcess->entryPoint(args);
    DebugConsole("Process %d returned to launch\n", runningProcess->pid);

    /* Stop the process gracefully */
    k_exit(result);
    return result;
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
    int result = 0;

    // check the running process to see if it has children
    if (runningProcess->pChildren == NULL)
    {
        return -4;
    }

    // check if the process has been signaled
    if (runningProcess->signal != 0)
    {
        return -5;
    }

    // change state to blocked
    runningProcess->status = BLOCKED;
    // remove the process from the running list
    removeFromProcessList(runningProcess, &processStatusList[RUNNING]);
    // add the process to the blocked list
    insertIntoProcessList(runningProcess, &processStatusList, BLOCKED, runningProcess->priority);
    // set the running process to NULL
    runningProcess = NULL;
    // call the dispatcher to switch to the next process
    dispatcher();

    // this process is resumed when a child quits
    // check the quit list for the child
    Process *nextProcess = &processStatusList[QUIT];
    while (1)
    {
        if (nextProcess == NULL)
        {
            continue;
        }
        if (nextProcess->pParent == runningProcess)
        {
            // remove the process from the quit list
            removeFromProcessList(nextProcess, &processStatusList[QUIT]);
            // remove the process from the process table
            removeFromProcessTable(nextProcess, &processTable, runningProcess, &processStatusList);
            // set the exit code
            *code = nextProcess->exitCode;
            result = nextProcess->pid;
            break;
        }
        nextProcess = nextProcess->nextSiblingProcess;
    }
    return result;
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
    // wake up the parent
    // print out who called exit
    console_output(debugFlag, "Process %s is exiting\n", runningProcess->name);

    // print out all the properties of the process
    console_output(debugFlag, "Process %s has the following properties\n", runningProcess->name);
    console_output(debugFlag, "PID: %d\n", runningProcess->pid);
    console_output(debugFlag, "Status: %d\n", runningProcess->status);
    console_output(debugFlag, "Priority: %d\n", runningProcess->priority);
    console_output(debugFlag, "Start Time: %d\n", runningProcess->startTime);
    console_output(debugFlag, "End Time: %d\n", runningProcess->endTime);
    console_output(debugFlag, "Elapsed Time: %d\n", runningProcess->elapsedTime);
    console_output(debugFlag, "Time Slice: %d\n", runningProcess->timeSlice);
    console_output(debugFlag, "Demotion Time: %d\n", runningProcess->demotionTime);
    console_output(debugFlag, "Demotion Count: %d\n", runningProcess->demotionCount);
    console_output(debugFlag, "Exit Code: %d\n", runningProcess->exitCode);

    // check if the process has children
    if (runningProcess->pChildren != NULL)
    {

        runningProcess->pParent->status = READY;
        // place the parent in the ready list
        insertIntoProcessList(runningProcess->pParent, &processStatusList, READY, runningProcess->pParent->priority);
        // remove the parent from the blocked list
        removeFromProcessList(runningProcess->pParent, &processStatusList[BLOCKED]);
    }

    // Update
    runningProcess->status = QUIT;
    runningProcess->exitCode = code;
    runningProcess->endTime = read_clock();
    runningProcess->elapsedTime = runningProcess->endTime - runningProcess->startTime;

    // stop the process
    context_stop(runningProcess->context);

    // call the dispatcher to switch to the next process
    dispatcher();

    // // remove the process from the running list
    // removeFromProcessList(runningProcess, &processList.headRunningProcessesPtr);
    // // add the process to the quit list
    // insertIntoProcessList(runningProcess, &processList.headQuitProcessesPtr, QUIT, runningProcess->priority);

    // // call the dispatcher to switch to the next process
    // dispatcher();
}

/**************************************************************************
   Name - k_kill

   Purpose - Signals a process with the specified signal

   Parameters - Signal to send

   Returns -
*************************************************************************/
int k_kill(int pid, int signal)
{
    int result = 0;
    return 0;
}

/**************************************************************************
   Name - k_getpid
*************************************************************************/
int k_getpid()
{
    if (runningProcess != NULL)
    {
        return runningProcess->pid;
    }
    return 0;
}

/**************************************************************************
   Name - k_join
***************************************************************************/
int k_join(int pid, int *pChildExitCode)
{
    int result = 0;

    return result;
}

/**************************************************************************
   Name - unblock
*************************************************************************/
int unblock(int pid)
{
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
    return 0;
}
/*************************************************************************
   Name - readtime
*************************************************************************/
int read_time()
{
    return 0;
}

/*************************************************************************
   Name - readClock
*************************************************************************/
DWORD read_clock()
{
    return system_clock();
}

void display_process_table()
{
}

/**************************************************************************
   Name - dispatcher

   Purpose - This is where context changes to the next process to run.

   Parameters - none

   Returns - nothing

*************************************************************************/
void dispatcher()
{
    disableInterrupts();
    Process *nextProcess = NULL;
    Process *tempProcess = NULL;

    /*
    This is where the kernel
    decides which process to run next and changes context to the next process to run if
    context needs to change from the currently running process.
*/

    // A process is running, we need to stop it
    if (runningProcess != NULL)
    {

        // console_output(debugFlag, "Process %s is running\n", runningProcess->name);
        // A process is already running, so we need to switch to another process
        tempProcess = runningProcess;
        // remove the running process from the running list
        removeFromProcessList(tempProcess, &processStatusList[RUNNING]);
        // add the process to the ready list
        insertIntoProcessList(tempProcess, &processStatusList, READY, tempProcess->priority);

        // check to see if the process has an end time, or has a status of QUIT
        if (tempProcess->status != QUIT)
        {
            // the process is not done, so we need to add it back to the ready list
            // before adding it back to the ready list we need to see if it has run for too long
            // if it has, then we need to lower its priority
            //- This is a rudimentary example to have something to work with
            if (tempProcess->elapsedTime >= (tempProcess->timeSlice) * tempProcess->priority)
            {
                // lower the priority of the process
                if (tempProcess->priority > 0)
                {
                    tempProcess->priority--;
                    tempProcess->demotionCount++;
                    tempProcess->elapsedTime = 0;
                }
            }

            // check if the process has been punished too many times
            // this should be based on the number of times the process has been demoted
            // and its demotion time - This is a rudimentary example to have something to work with
            if (tempProcess->demotionTime > tempProcess->timeSlice * tempProcess->demotionCount)
            {
                // the process has been punished too many times, so lets restore its priority
                tempProcess->priority += tempProcess->demotionCount;
                tempProcess->demotionCount = 0;
                tempProcess->demotionTime = 0;
            }

            // add the process to the appropriate list based on its status
            // should be READY but if the process became blocked then it will be added to the blocked list
            switch (tempProcess->status)
            {
                console_output(debugFlag, "Process status is %s\n", tempProcess->name);
            case READY:
                insertIntoProcessList(tempProcess, &processStatusList, READY, tempProcess->priority);
                break;
            case BLOCKED:
                insertIntoProcessList(tempProcess, &processStatusList, BLOCKED, tempProcess->priority);
                break;
            case QUIT:
                insertIntoProcessList(tempProcess, &processStatusList, QUIT, tempProcess->priority);
                // if the process has a parent, and the parent is blocked, then we need to unblock the parent
                if (tempProcess->pParent != NULL && tempProcess->pParent->status == BLOCKED)
                {
                    tempProcess->pParent->status = READY;
                    // remove the parent from the blocked list
                    removeFromProcessList(tempProcess->pParent, &processStatusList[BLOCKED]);
                    insertIntoProcessList(tempProcess->pParent, &processStatusList, READY,
                                          tempProcess->pParent->priority);
                }
                break;
            default:
                console_output(debugFlag, "Invalid process status for process %s\n", tempProcess->name);
            }
        }
        else
        {
            // print a message that the process has quit
            console_output(debugFlag, "Process status is QUIT\n");
        }
        // else - I am assuming**
        // do nothing, processes that have or are in a quit state will manage their own
        // cleanup and removal from the process list. This allows each process to handle
        // their children. This will be managed in the launch function after the process
        // has finished executing. We can check if the process has children and wait for them
        // if they are still running.
    }

    // remove the process that is at the head of the ready list, with the highest priority
    // The lists are ordered by priority, and are sorted on insertion
    nextProcess = &processStatusList[READY];
    runningProcess = nextProcess;

    // if there is no process to run, then some error has occurred
    if (nextProcess == NULL)
    {
        console_output(debugFlag, "No processes to run, stopping the system...\n");
        stop(1);
    }
    else
    {
        // This process needs to be removed from the ready list and added to the running list[sets its status to running]
        removeFromProcessList(nextProcess, &processStatusList[READY]);
        insertIntoProcessList(nextProcess, &processStatusList, RUNNING, nextProcess->priority);

        // check if the process has a start time, if not set it
        if (runningProcess->startTime == 0)
        {
            runningProcess->startTime = system_clock();
        }

        enableInterrupts();
        /* IMPORTANT: context switch enables interrupts. */
        context_switch(nextProcess->context);
    }
}

/**************************************************************************
   Name - watchdog

   Purpose - The watchdoog keeps the system going when all other
         processes are blocked.  It can be used to detect when the system
         is shutting down as well as when a deadlock condition arises.

   Parameters - none

   Returns - nothing
   *************************************************************************/
static int watchdog(char *dummy)
{
    DebugConsole("watchdog(): called\n");
    while (1)
    {
        check_deadlock();
    }
    return 0;
}

/* check to determine if deadlock has occurred... */
static void check_deadlock()
{
}

/*
 * Disables the interrupts.
 */
static inline void disableInterrupts()
{

    /* We ARE in kernel mode */

    int psr = get_psr();

    psr = psr & ~PSR_INTERRUPTS;

    set_psr(psr);

} /* disableInterrupts */

/*
 * Enables the interrupts.
 */
static inline void enableInterrupts()
{

    int psr = get_psr();

    psr = psr | PSR_INTERRUPTS;

    set_psr(psr);

} /*

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
    return false;
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
    static int elapsed = 0;           // time elapsed since last context switch
    static int lastTime = 0;          // last time the clock interrupt was called
    int currentTime = system_clock(); // current time in ms- the documentation says its in microseconds μs but
                                      // the week1 THREADSDemo says 1000 = 1 second so it has to be in milliseconds
    int minQuantum = 20;              // 20 ms , should be 20-50 ms according to the book

    // if there is a running process and it doesnt have a start time, set it
    if (runningProcess != NULL && runningProcess->startTime == 0)
    {
        runningProcess->startTime = currentTime;
    }

    // calculate the time elapsed since the last context switch
    if (lastTime != 0)
    {
        elapsed += (currentTime - lastTime);
    }

    // update the last time the clock interrupt was called
    lastTime = currentTime;

    // if there is a running process, update its elapsed time
    if (runningProcess != NULL)
    {
        runningProcess->elapsedTime += elapsed;
    }

    // check if the elapsed time is greater than the minimum quantum
    if (elapsed >= minQuantum)
    {
        elapsed = 0;
        dispatcher();
    }
}