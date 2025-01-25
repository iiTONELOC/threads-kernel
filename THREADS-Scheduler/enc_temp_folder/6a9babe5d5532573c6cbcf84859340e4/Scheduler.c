#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "THREADSLib.h"
#include "Constants.h"
#include "Scheduler.h"
#include "Processes.h"

int nextPid = 1;
int debugFlag = 1;
int inBootStrap = 0;

Process *runningProcess = NULL;
Process processTable[MAX_PROCESSES];
interrupt_handler_t *interruptHandlers = NULL;

void time_slice();
void dispatcher();
static int launch(void *);
static int watchdog(char *);
static void check_deadlock();
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

    /* Initialize the process table. */
    InitializeProcessTable(processTable, MAX_PROCESSES);
    console_output(debugFlag, "init(): Process table initialized with %d entries\n", MAX_PROCESSES);

    /* Initialize the Ready list, etc. */

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
    int proc_slot;
    struct _process *pNewProc;

    DebugConsole("spawn(): creating process %s\n", name);

    disableInterrupts();

    /* Validate all of the parameters, starting with the name. */
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

    /* Find an empty slot in the process table */

    proc_slot = 1; // just use 1 for now!
    pNewProc = &processTable[proc_slot];

    /* Setup the entry in the process table. */
    strcpy(pNewProc->name, name);

    /* If there is a parent process,add this to the list of children. */
    if (runningProcess != NULL)
    {
    }

    /* Add the process to the ready list. */

    /* Initialize context for this process, but use launch function pointer for
     * the initial value of the process's program counter (PC)
     */
    pNewProc->context = context_initialize(launch, stacksize, arg);

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

    DebugConsole("launch(): started: %s\n", runningProcess->name);

    /* Enable interrupts */

    /* Call the function passed to spawn and capture its return value */
    DebugConsole("Process %d returned to launch\n", runningProcess->pid);

    /* Stop the process gracefully */

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
    int result = 0;
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
    return 0;
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
    Process *nextProcess = NULL;

    /* IMPORTANT: context switch enables interrupts. */
    context_switch(nextProcess->context);
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
 * @brief Time slice function
 *
 * The time_slice function determines if the currently active process has exceeded its
 * current time slice. If the quantum value has been exceeded the dispatcher is called
 */
void time_slice()
{
    if (inBootStrap)
    {
        return;
    }

    static int elapsed = 0;                          // time elapsed since last context switch
    static int lastTime = 0;                         // last time the clock interrupt was called
    int currentTime = system_clock();                // current time in μs but
                                                     // the week1 THREADSDemo says 1000 = 1 second so it has to be in milliseconds
    int minQuantumUs = MIN_TIME_SLICE_MS * MS_TO_US; // 20 ms , should be 20-50 ms according to the book

    // if there is a running process and it doesn't have a start time, set it
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

    // if there is a running process, update its elapsed and cpu time
    if (runningProcess != NULL)
    {
        runningProcess->elapsedTime += elapsed;
        runningProcess->cpuTime += elapsed;
    }

    // check if the elapsed time is greater than the minimum quantum
    if (elapsed >= minQuantumUs)
    {
        elapsed = 0;
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
