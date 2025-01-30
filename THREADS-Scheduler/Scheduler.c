#define _CRT_SECURE_NO_WARNINGS

// various headers may already import stdio.h, so set the IMPORT_STDIO flag
#ifndef IMPORT_STDIO
#define IMPORT_STDIO
#include <stdio.h>
#endif
#include "THREADSLib.h"
#include "Constants.h"
#include "Scheduler.h"
#include "Processes.h"
#include "StringUtils.h"
#include "PriorityProcessQueue.h"

int nextPid = 1;                                        // next process id
int debugFlag = 0;                                      // debug flag
int inBootStrap = 0;                                    // flag to indicate if the system is in bootstrap
Process *runningProcess = NULL;                         // tracks current running process
Process processTable[MAX_PROCESSES];                    // process table
interrupt_handler_t *interruptHandlers = NULL;          // interrupt handlers from THREADS API
DoublyLinkedList priorityListQueue[NUM_PROCESS_STATES]; // Priority list queue for process states
DoublyLinkedNode procTableListBucket[MAX_PROCESSES];    // Storage for process state linked list

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

    /*
        set the inBootStrap flag to true - this will ensure the dispatcher does
        not run until the system is ready to go. This is important since
        k_spawn() will call the dispatcher after the context for the process is
        initialized.
    */
    inBootStrap = 1;

    /* set this to the scheduler version of this function.*/
    check_io = check_io_scheduler;

    /* Initialize the process table */
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        InitializeProcessToDefault(&processTable[i]);
        InitializeDoublyLinkedNode(&procTableListBucket[i]);
    }

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

    /* start the test process, which is the main for each test program.  */
    result = k_spawn("Scheduler", SchedulerEntryPoint, NULL, 2 * THREADS_MIN_STACK_SIZE, HIGHEST_PRIORITY);
    if (result < 0)
    {
        console_output(debugFlag,
                       "Scheduler(): spawn for SchedulerEntryPoint returned an error (%d), stopping...\n", result);
        stop(1);
    }

    /* set the inBootStrap flag to false - this will allow the dispatcher to run */
    inBootStrap = 0;
    // runningProcess is null
    // the dispatcher will look for the next ready process or call the watchdog
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

    // Get the process structure from the process table
    pNewProc = &processTable[proc_slot];

    /* Setup the entry in the process table. */
    pNewProc->signal = 0;                      // set the signal to an initial value
    pNewProc->cpuTime = 0;                     // set the cpu time to an initial value
    pNewProc->pid = nextPid;                   // set process id
    pNewProc->startTime = 0;                   // set the start time to an initial value
    pNewProc->status = READY;                  // set the status to ready
    pNewProc->elapsedTime = 0;                 // set the elapsed time to an initial value
    pNewProc->priority = priority;             // set process priority
    pNewProc->stacksize = stacksize;           // set process stack size
    pNewProc->entryPoint = entryPoint;         // set process entry point
    pNewProc->processTableIndex = proc_slot;   // set the table index
    pNewProc->quantum = DEFAULT_TIME_SLICE_MS; // set the time slice
    CopyString(name, pNewProc->name, MAXNAME); // Copy process name - shouldn't be NULL
    // Copy process arguments - might be NULL
    if (arg != NULL)
    {
        CopyString(arg, pNewProc->startArgs, MAXARG);
    }
    else
    {
        pNewProc->startArgs[0] = '\0'; // Empty string if no arguments
    }

    // add the process to node storage and the ready list
    procTableListBucket[proc_slot].pData = pNewProc;
    AddNodeToPriorityProcessQueue(priorityListQueue, &procTableListBucket[proc_slot]);

    /* If there is a parent process, add this to its list of children. */
    if (runningProcess != NULL)
    {
        // dynamically allocate a new linked list node for the parent's children list
        // this keeps from mutating the priority list queue
        pNewChildProcNode = CreateDoublyLinkedNode(pNewProc);

        // if the linked list node is NULL, return an error
        if (pNewChildProcNode == NULL)
        {
            console_output(debugFlag, "spawn(): Error: Could not create a new linked list node for the child process.\n");
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
    DoublyLinkedNode *pNextNode;        // Used for traversal
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

    // Check to see if there is a dead child
    if (runningProcess->pDeadChildren.count > 0)
    {
        // FIFO - removes the first child from the list
        // TODO: FIX THIS so that if a parent detects that a child quit before it called wait,
        // it needs to clean up the child process and return the exit code
        // RemoveDoublyLinkedNode(&runningProcess->pDeadChildren, runningProcess->pDeadChildren.pHead);
        console_output(debugFlag, "k_wait(): Error: Dead Child Found! - PRE-BLOCK - Likely a child w/higher priority than parent!\n");
    }
    else
    {
        // Get the linked list node for the running process
        pTempNode = FindProcessNodeByPid(runningProcess->pid, procTableListBucket);

        // This should not happen, but if it does, return an error
        if (pTempNode == NULL)
        {
            console_output(debugFlag, "k_wait(): Error: Corresponding Linked List Node for Process was not found!\n");
            return -1;
        }

        // remove the process from the ready list and place it in the blocked list
        ChangeProcessStatus(priorityListQueue, pTempNode, BLOCKED);

        // set the running process to NULL so the dispatcher can find the next process to run
        runningProcess = NULL;

        // call the dispatcher to update the running process to blocked
        dispatcher();
    }

    // AFTER BLOCK - Parent needs to clean up for children
    // look in the Quit list for a child of this
    pTempNode = runningProcess->pDeadChildren.pHead; // first child in the running process's dead children list
    pChild = (Process *)pTempNode->pData;            // get the process from the linked list node

    while (pTempNode != NULL)
    {
        if (pChild == NULL)
        {
            console_output(debugFlag, "k_wait(): Error: pChild not found!\n");
            return -1;
        }

        // check for quit status
        if (pChild->status == QUIT)
        { // check for signal
            if (pChild->signal)
            {
                return -5;
            }

            // quitting child found, not signaled
            // set the exit code
            *code = pChild->exitCode;

            // set the pid to the result
            result = pChild->pid;
            break;
        }
    }

    if (result == 0)
    {
        console_output(debugFlag, "k_wait(): Error: No child found in the quit list!\n");
        return -4;
    }

    pNextNode = FindProcessNodeByPid(result, procTableListBucket);
    // remove child from quit list - HAVE to use the status to remove the child from the quit list
    RemoveDoublyLinkedNode(&priorityListQueue[QUIT], pNextNode);
    // remove the child from the parent's children list - Dynamic allocation
    RemoveDoublyLinkedNode(&runningProcess->pDeadChildren, pTempNode);
    // reinitialize the process structure
    InitializeProcessToDefault(pChild);
    // reinitialize the linked list node by setting all values to NULL
    InitializeDoublyLinkedNode(pNextNode);
    // free the memory for the linked list node
    DestroyDoublyLinkedNode(pTempNode);

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
    disableInterrupts();
    DoublyLinkedNode *pListNode = NULL;

    // terminate the process and set its exit code
    // set the status to QUIT
    if (runningProcess == NULL)
    {
        console_output(debugFlag, "k_exit(), running process not found!!\n");
        stop(1);
    }

    // check for a signal event
    if (runningProcess->signal)
    {
        // if the process was signaled, set the exit code to the signal
        code = runningProcess->signal;
    }

    // set the exit code
    runningProcess->exitCode = code;
    // Grab the linked list node for the process
    pListNode = FindProcessNodeByPid(runningProcess->pid, procTableListBucket);

    // set the status to QUIT
    ChangeProcessStatus(priorityListQueue, pListNode, QUIT);

    // UNBLOCK PARENT IF PARENT EXISTS
    if (runningProcess->pParent != NULL)
    {
        // check if the parent is blocked before changing the status
        // if the parent is still running, we have a child process
        // with a higher priority than the parent process
        if (((Process *)runningProcess->pParent)->status == BLOCKED)
        {
            ChangeProcessStatus(priorityListQueue, FindProcessNodeByPid(runningProcess->pParent->pid, procTableListBucket), READY);
        }

        // place into parent's dead children list
        // but do not change the status
        RemoveDoublyLinkedNode(&runningProcess->pParent->pChildren, pListNode);
        InsertDoublyLinkedNode(&runningProcess->pParent->pDeadChildren, pListNode);
    }
    // set the current running process to NULL
    runningProcess = NULL;
    // call the dispatcher
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
    // disableInterrupts();
    // get the process from the process table
    DoublyLinkedNode *pListNode = FindProcessNodeByPid(pid, procTableListBucket);

    // if invalid or process is not blocked, return -1
    if (pListNode == NULL || ((Process *)pListNode->pData)->status != BLOCKED)
    {
        return -1;
    }

    ChangeProcessStatus(priorityListQueue, pListNode, READY);
    // enableInterrupts();
    // TODO: Call dispatch??

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
    Process *pCurrentProc = NULL;
    DoublyLinkedNode *pNextLNode = NULL;

    // if we are in bootstrap, we need to return
    if (inBootStrap)
    {
        return;
    }

    // check to see if the current process has exceeded its quantum
    if (runningProcess != NULL && runningProcess->elapsedTime < MIN_TIME_SLICE_MS)
    {
        return;
    }

    // get the next ready process
    pNextLNode = priorityListQueue[READY].pHead;

    // if there are no ready processes, call the watchdog
    if (pNextLNode == NULL || pNextLNode->pData == NULL)
    {
        watchdog(NULL);
    }

    // change the status of the running process to ready
    if (runningProcess != NULL)
    {
        ChangeProcessStatus(priorityListQueue, FindProcessNodeByPid(runningProcess->pid, procTableListBucket), READY);
    }

    // get the next process to run
    runningProcess = (Process *)pNextLNode->pData;

    // set its status to running
    ChangeProcessStatus(priorityListQueue, pNextLNode, RUNNING);
    // set the running process to the current process
    context_switch(runningProcess->context);
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
    DoublyLinkedNode *pNode = priorityListQueue[READY].pHead;

    if (inBootStrap)
    {
        // We are still booting up, so we need to wait for the system to be ready
        return 0;
    }

    if (pNode == NULL || pNode->pData == NULL)
    {
        stop(0);
    }

    pNextReadyProc = (Process *)pNode->pData;

    // if there are no processes and we are not in bootstrap
    // we need to make sure there are NO processes in a BLOCKED, QUIT, or READY state
    if (pNextReadyProc == NULL)
    {
        while (1)
        {
            check_deadlock();
        }
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
    for (i = RUNNING, i <= BLOCKED; i++;)
    {
        // check the count on each linked list
        // to determine if there are processes in the system
        if (priorityListQueue[i].count > 0)
        {
            return;
        }
    }

    // if we are here, then there are no processes in the system
    // and we are not in bootstrap
    console_output(debugFlag, "check_deadlock(): no processes detected, stopping...\n");

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

    static int elapsed = 0;           // time elapsed since last context switch
    static int lastTime = 0;          // last time the clock interrupt was called
    int currentTime = system_clock(); // current time in μs but
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
