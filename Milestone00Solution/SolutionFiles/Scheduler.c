
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "THREADSLib.h"
#include "Constants.h"
#include "Scheduler.h"
#include "Processes.h"

#include "PriorityProcessQueue.h"

int nextPid = 1;                                        // next process id
int debugFlag = 0;                                      // debug flag
int isBooting = 0;                                      // flag to indicate if the system is in bootstrap
Process *runningProcess = NULL;                         // tracks current running process
Process processTable[MAX_PROCESSES];                    // process table
interrupt_handler_t *interruptHandlers = NULL;          // interrupt handlers from THREADS API
DoublyLinkedNode staticNodeStorage[MAX_PROCESSES];      // Storage for process state linked list
DoublyLinkedList priorityListQueue[NUM_PROCESS_STATES]; // Priority list queue for process states

int cpu_time();
void time_slice();
void dispatcher();
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

    isBooting = 1; // set isBooting flag to true - Won't allow the dispatcher to run

    /* set this to the scheduler version of this function.*/
    check_io = check_io_scheduler;

    /* Initialize the process table */
    InitializeProcessTable(processTable, MAX_PROCESSES);
    /* Initialize the process table linked list bucket */
    InitializeDoublyLinkedNodeStorage(staticNodeStorage, MAX_PROCESSES);
    /* Initialize the priority list queue */
    InitializePriorityProcessQueueArray(priorityListQueue, NUM_PROCESS_STATES);

    /* Initialize the clock interrupt handler */
    interruptHandlers = get_interrupt_handlers();
    interruptHandlers[THREADS_TIMER_INTERRUPT] = (interrupt_handler_t)&clockInterruptHandler;

    /* startup a watchdog process */
    result = k_spawn("watchdog", watchdog, NULL, THREADS_MIN_STACK_SIZE, LOWEST_PRIORITY);
    if (result < 0)
    {
        console_output(debugFlag,
                       "Scheduler(): spawn for watchdog returned an error (%d), stopping...\n", result);
        stop(1);
    }

    isBooting = 0; // set isBooting flag to false - Will allow the dispatcher to run
    /* start the test process, which is the main for each test program.  */
    result = k_spawn("Scheduler", SchedulerEntryPoint, NULL, 2 * THREADS_MIN_STACK_SIZE, HIGHEST_PRIORITY);
    if (result < 0)
    {
        console_output(debugFlag,
                       "Scheduler(): spawn for SchedulerEntryPoint returned an error (%d), stopping...\n", result);
        stop(1);
    }

    // Should not get here, the dispatcher should start from the last spawn call

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
    int result = 0;
    int proc_slot;
    Process *pNewProc;
    DoublyLinkedNode *pNewChildProcNode = NULL;
    DoublyLinkedNode *pRunningProcessLinkedListNode = NULL;

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

    if (arg != NULL && strlen((char *)arg) >= (MAXARG - 1))
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
    proc_slot = GetEmptyControlBlockIndex(processTable);

    if (proc_slot < 0)
    {
        console_output(debugFlag, "spawn(): No empty slots in the process table.\n");
        return -4;
    }

    // Get a pointer to the new process
    pNewProc = &processTable[proc_slot];

    /* Setup the new process in the process table. */
    InitializeNewProcess(pNewProc, name, entryPoint, arg, stacksize, priority,
                         proc_slot, nextPid);

    // add the process to static node storage (for priority list) and the priority list queue
    staticNodeStorage[proc_slot].pData = pNewProc;
    AddNodeToPriorityProcessQueue(priorityListQueue, &staticNodeStorage[proc_slot]);

    /* If there is a parent process, add this to its list of children. */
    if (runningProcess != NULL)
    {
        // dynamically allocate a new linked list node for the parent's children list
        // this keeps from mutating the priority list queue
        pNewChildProcNode = CreateDoublyLinkedNode(pNewProc);

        // if the linked list node is NULL, return an error
        if (pNewChildProcNode == NULL)
        {
            console_output(debugFlag,
                           "spawn(): Error: Could not create a new linked list node for the child process.\n");
            return -6;
        }

        // add the child process to the parent's children list
        InsertDoublyLinkedNode(&runningProcess->pChildren, pNewChildProcNode);

        // add the parent process to the child process' pParent
        pNewProc->pParent = runningProcess;
    }

    /* Initialize context for this process, but use launch function pointer for
     * the initial value of the process's program counter (PC)
     */
    pNewProc->context = context_initialize(launch, stacksize, pNewProc->startArgs);

    /* Increment the process id for the next process */
    nextPid++;

    // call the dispatcher
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
    int result = 0;
    /* Enable interrupts */
    enableInterrupts();

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
    disableInterrupts();
    int result = 0;                     // return value
    Process *pChild = NULL;             // Child process
    DoublyLinkedNode *pTempNode = NULL; // Linked List Node for the Running Process

    // Look for a running process, if there is a running process then it is the parent of the
    // current process that is trying to exit.
    if (runningProcess == NULL)
    {
        console_output(debugFlag, "k_wait(): Error: Running process not found!");
        return -1;
    }

    // if the process was signaled, return -5
    if (runningProcess->signal)
    {
        return -5;
    }
  
    // check for dead children - children that have already quit
    if (runningProcess->pDeadChildren.count > 0)
    {
        CleanUpAfterChild(runningProcess,
                          &runningProcess->pDeadChildren,
                          staticNodeStorage,
                          priorityListQueue,
                          code,
                          &result);
        enableInterrupts();
        return result;
    }

    // set status to blocked and call the dispatcher
    ChangeProcessStatus(priorityListQueue,
                        FindDoublyLinkedNode(runningProcess,
                                             &priorityListQueue[STATUS_RUNNING]),
                        STATUS_BLOCKED_WAIT);

    dispatcher();

    // _______________AFTER PARENT WAS AWAKENED BY THEIR CHILD____________________
    
    disableInterrupts();
    // get the exit code of the child
    if (runningProcess->pExitingChildren.count > 0)
    {
        CleanUpAfterChild(runningProcess,
                          &runningProcess->pExitingChildren,
                          staticNodeStorage,
                          priorityListQueue,
                          code,
                          &result);
    }
    else
    {
        console_output(debugFlag, "k_wait(): Exiting child not found in the exiting children list\n");
    }

  /*  enableInterrupts();*/
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

    DoublyLinkedNode *pDynamicNode = NULL;
    DoublyLinkedNode *pStaticListNode = NULL;

    // terminate the process and set its exit code
    // set the status to QUIT
    if (runningProcess == NULL)
    {
        console_output(debugFlag, "k_exit(), running process not found!!\n");
        stop(1);
    }

    // check if the process has children
    if (runningProcess->pChildren.count > 0 ||
        runningProcess->pDeadChildren.count > 0)
    {
        console_output(debugFlag, "k_exit(): Error: Process has children!\n");
        stop(1);
    }

    // check for a signal event - Currently only SIG_TERM is supported
    if (runningProcess->signal == SIG_TERM)
    {
        // if the process was signaled, set the exit code to the signal
        runningProcess->exitCode = -5;
    }
    else
    {
        // set the exit code
        runningProcess->exitCode = code;
    }
    disableInterrupts();
    // Grab the static linked list node for the process from static node storage
    pStaticListNode = FindDoublyLinkedNode(runningProcess, &priorityListQueue[STATUS_RUNNING]);

    // set the status of the quitting process to QUIT - requires a static node
    ChangeProcessStatus(priorityListQueue, pStaticListNode, STATUS_QUIT);

    // The process has a parent so we need to inform the parent that the child has quit
    if (runningProcess->pParent != NULL)
    {
        pDynamicNode = FindDoublyLinkedNode(runningProcess, &runningProcess->pParent->pChildren);
        // check if the parent is blocked before changing the status
        // if the parent is still running, we have a child process
        // with a higher priority than the parent process
        if (((Process *)runningProcess->pParent)->status == STATUS_BLOCKED_WAIT)
        {
            // // change the status of the parent to ready
            ChangeProcessStatus(priorityListQueue,
                                FindDoublyLinkedNode(runningProcess->pParent,
                                                     &priorityListQueue[STATUS_BLOCKED_WAIT]),
                                STATUS_READY);

            // place the child into parent's exiting children list
            // but do not change the status of the child from QUIT
            RemoveDoublyLinkedNode(&runningProcess->pParent->pChildren, pDynamicNode);
            InsertDoublyLinkedNode(&runningProcess->pParent->pExitingChildren, pDynamicNode);
        }
        else
        {
            // place the child into parent's dead children list
            // but do not change the status of the child from QUIT
            RemoveDoublyLinkedNode(&runningProcess->pParent->pChildren, pDynamicNode);
            InsertDoublyLinkedNode(&runningProcess->pParent->pDeadChildren, pDynamicNode);
        }
    }
    else
    {

        // clean up the process since it has no parent - it needs to manage itself
        // not a child clean up the process
        // reinitialize the process structure
        InitializeProcessToDefault(runningProcess);
        // reinitialize the linked list node by setting all values to NULL
        InitializeDoublyLinkedNode(pStaticListNode);
    }

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

    int result = 0;
    DoublyLinkedNode *pListNode = NULL;

    // look for the process in the process table
    pListNode = FindStaticStorageNode(pid, staticNodeStorage);

    // if the process is not found or signal is not equal to SIG_TERM
    if (pListNode == NULL ||
        signal != SIG_TERM ||
        (pListNode != NULL && ((Process *)pListNode->pData) == NULL))
    {
        stop(1);
    }

    // set the signal for the process
    ((Process *)pListNode->pData)->signal = signal;
    return 0;
}

/**************************************************************************
   Name - k_getpid
*************************************************************************/
int k_getpid()
{
    return runningProcess ? runningProcess->pid : 0;
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
    disableInterrupts();
    // get the process from the process table
    DoublyLinkedNode *pListNode = FindStaticStorageNode(pid, staticNodeStorage);

    // if invalid or process is not blocked, return -1
    if (pListNode == NULL || ((Process *)pListNode->pData)->status != STATUS_BLOCKED_WAIT)
    {
        return -1;
    }

    ChangeProcessStatus(priorityListQueue, pListNode, STATUS_READY);
    dispatcher();
    return 0;
}

/*************************************************************************
   Name - block
*************************************************************************/
int block(int newStatus)
{
    // function blocks the calling process and sets the status in the process table to the value specified by newStatus
    // disableInterrupts();
    // // if the new status is between 0-10
    // if (newStatus < 0 || newStatus < 10)
    // {
    //     console_output(debugFlag, "block(): Error: Invalid status value!\n");
    //     stop(1);
    // }

    // ChangeProcessStatus
    // dispatcher();
    return 0;
}

/*************************************************************************
   Name - signaled
*************************************************************************/
int signaled()
{
    return (runningProcess && runningProcess->signal) ? 1 : 0;
}
/*************************************************************************
   Name - readtime
*************************************************************************/
int read_time()
{
    return cpu_time();
}
/*************************************************************************
   Name - cpu_time
*************************************************************************/
int cpu_time()
{
    return runningProcess ? runningProcess->cpuTime : 0;
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

    /**
     *  In C - Variables should be declared before you use them.
     *    This is different from Python because in Python there is no way to declare a variable.
     *
     *    Also, you shouldn't decare variables inside of a loop,
     *    because it will be redeclared every time the loop runs
     *
     *    Declare them outside the loop and update the values as needed
     *
     * It is always a good practice to declare variables at the beginning of the function
     * it is up for debate on variable initialization, I feel for best practice and to ensure
     * that you dont have any garbage values, you should initialize your variables when declaring them
     * */

    int parentPid = -1;
    int numChildren = 0;
    char *statusStr = NULL;
    Process *pProcess = NULL;
    char statusBuffer[20] = {0}; // Buffer for converting status number to a string

    // Print header
    console_output(FALSE, "PID     Parent   Priority  Status         # Kids   CPUtime  Name\n");

    // Loop through process table
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        pProcess = &processTable[i];
        // Skip empty process slots
        if (pProcess->context == NULL)
        {
            continue;
        }

        if (pProcess->status == STATUS_READY)
        {
            statusStr = "READY";
        }
        else if (pProcess->status == STATUS_RUNNING)
        {
            statusStr = "RUNNING";
        }
        else if (pProcess->status == STATUS_BLOCKED_WAIT)
        {
            statusStr = "WAIT BLOCK";
        }
        else if (pProcess->status == STATUS_QUIT)
        {
            statusStr = "QUIT";
        }
        else if (pProcess->status == 14)
        { // Special case for JOIN BLOCK
            statusStr = "JOIN BLOCK";
        }
        else if (pProcess->status > 10)
        {
            snprintf(statusBuffer, sizeof(statusBuffer), "%d", pProcess->status);
            statusStr = statusBuffer;
        }
        else
        {
            statusStr = "UNKNOWN";
        }

        // Get parent PID

        if (pProcess->pParent != NULL)
        {
            parentPid = pProcess->pParent->pid;
        }
        else
        {
            parentPid = -1;
        }

        // Count number of children
        // Might need to count the other lists as well
        numChildren = pProcess->pChildren.count;

        // Print process information
        console_output(FALSE, "%-8d%-9d%-11d%-14s%-9d%-9llu%s\n",
                       pProcess->pid,
                       parentPid,
                       pProcess->priority,
                       statusStr,
                       numChildren,
                       pProcess->cpuTime,
                       pProcess->name);
    }
    console_output(FALSE, "\n");
}

/**************************************************************************
   Name - dispatcher

   Purpose - This is where context changes to the next process to run.

   Parameters - none

   Returns - nothing

*************************************************************************/
void dispatcher()
{

    Process *pNextReadyProcess = NULL;

    // if we are in bootstrap, we need to return
    if (isBooting)
    {
        return;
    }

    disableInterrupts();
    // get the next ready process
    pNextReadyProcess = GetNextReadyProcess(runningProcess, priorityListQueue);

    // if the next Ready process is null
    if (pNextReadyProcess != NULL)
    {
        // if we get the same process back no need to context switch
        if (runningProcess != NULL &&
            pNextReadyProcess->pid == runningProcess->pid)
        {

            return;
        }

        // set the running process to the next ready process
        runningProcess = pNextReadyProcess;

        // set the running process to the current process
        context_switch(runningProcess->context);
    }
    else
    {

        watchdog(NULL);
    }
}

/**************************************************************************
   Name - watchdog

   Purpose - The watchdog keeps the system going when all other
         processes are blocked.  It can be used to detect when the system
         is shutting down as well as when a deadlock condition arises.

   Parameters - none

   Returns - nothing
   *************************************************************************/
static int watchdog(void *dummy)
{
    Process *pNextReadyProc = NULL;
    DoublyLinkedNode *pNode = priorityListQueue[STATUS_READY].pHead;

    if (isBooting)
    {
        // We are still booting up, so we need to wait for the system to be ready
        return 0;
    }

    if (pNode == NULL || pNode->pData == NULL)
    {
        check_deadlock();
        console_output(FALSE, "All processes completed.\n");
        // reset the tables
        InitializeProcessTable(processTable, MAX_PROCESSES);
        InitializePriorityProcessQueueArray(priorityListQueue, NUM_PROCESS_STATES);

        stop(0);
    }

    return 0;
}

/* check to determine if deadlock has occurred... */
static void check_deadlock()
{
    int i = 0;
    Process *pCurrentProc = NULL;
    DoublyLinkedNode *pNextLNode = NULL;

    // determine why the watchdog was called
    for (i = STATUS_RUNNING, i <= NUM_PROCESS_STATES - 1; i++;)
    {
        // check the count on each linked list
        // to determine if there are processes in the system
        if (priorityListQueue[i].count > 0)
        {
            return;
        }
    }
    // we arrived here because there are no processes in the system
    stop(1);
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
 * @brief Time slice function
 *
 * The time_slice function determines if the currently active process has exceeded its
 * current time slice. If the quantum value has been exceeded the dispatcher is called
 */
void time_slice()
{
    static int elapsed = 0;                                            // time elapsed since last context switch
    static int lastTime = 0;                                           // last time the clock interrupt was called
    int currentTime = system_clock();                                  // current time in μs but
    int minQuantumUs = 25 * NUM_MILLI_SEC_IN_MICRO_SEC; // 20 ms , should be 20-50 ms according to the book

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
