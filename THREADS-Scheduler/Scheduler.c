#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "THREADSLib.h"
#include "Constants.h"
#include "Scheduler.h"
#include "Processes.h"
#include "SchedulerUtils.h"
#include "DoublyLinkedList.h"
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

/**
 * @brief The bootstrap function for the scheduler
 *
 * @param pArgs The arguments to pass to the function
 * @return int The return value of the function
 *
 * @note This function is the first function called by THREADS on startup.
 *       This function must setup the OS scheduler and primitive functionality
 *
 *      The first two processes are the watchdog and the scheduler entry point
 *
 *      This function is used to init higher layers of the os and assist in testing
 *      the scheduler functions.
 *
 */
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

int k_spawn(char *name, int (*entryPoint)(void *), void *arg, int stacksize, int priority)
{
    enforceKernelMode();
    int result = 0;
    int proc_slot;
    Process *pNewProc;
    DoublyLinkedNode *pNewChildProcNode = NULL;
    DoublyLinkedNode *pRunningProcessLinkedListNode = NULL;

    result = ValidateKSpawnParams(name, entryPoint, arg, stacksize, priority, debugFlag);
    if (result != 0)
    {
        return result;
    }

    disableInterrupts();
    /* Find an empty slot in the process table */
    proc_slot = GetEmptyControlBlockIndex(processTable);

    if (proc_slot < 0)
    {
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
            return -1;
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
}

/**
 * @brief Utility function that makes sure the environment is ready
 *
 * @param args The arguments to pass to the function
 * @return int The return value of the function
 */
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

int k_wait(int *code)
{
    enforceKernelMode();
    disableInterrupts();
    int result = 0;                     // return value
    Process *pChild = NULL;             // Child process
    DoublyLinkedNode *pTempNode = NULL; // Linked List Node for the Running Process

    // Look for a running process, if there is a running process then it is the parent of the
    // current process that is trying to exit.

    // check for dead children - children that have already quit
    if (runningProcess != NULL && runningProcess->pDeadChildren.count > 0)
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

    if (runningProcess != NULL && runningProcess->pChildren.count == 0)
    {
        // no processes to wait for
        return -1;
    }

    ChangeProcessStatus(priorityListQueue,
                        FindDoublyLinkedNode(runningProcess,
                                             &priorityListQueue[STATUS_RUNNING]),
                        STATUS_BLOCKED_WAIT);
    runningProcess = NULL;

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
    if (runningProcess->signal)
    {
        return -5;
    }
    enableInterrupts();

    return result;
}

void k_exit(int code)
{
    enforceKernelMode();
    disableInterrupts();
    Process *pProcessINeedToJoin = NULL;
    DoublyLinkedNode *pDynamicNode = NULL;
    DoublyLinkedNode *pStaticListNode = NULL;

    // terminate the process and set its exit code
    // set the status to QUIT

    // check if the process has children
    if (runningProcess->pChildren.count > 0)
    {
        console_output(debugFlag, "quit(): Process with active children attempting to quit\n");
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

    // Grab the static linked list node for the process from static node storage
    pStaticListNode = FindDoublyLinkedNode(runningProcess, &priorityListQueue[STATUS_RUNNING]);

    // set the status of the quitting process to QUIT - requires a static node
    ChangeProcessStatus(priorityListQueue, pStaticListNode, STATUS_QUIT);

    // check if the process needs to join a process
    if (runningProcess->pJoiningProcesses.count > 0)
    {
        // console_output(debugFlag, "k_exit(): Process is joining another process\n");
        // for each process that is joining this process - set the status to ready
        // and remove the process from the joining list
        while (runningProcess->pJoiningProcesses.count > 0)
        {
            pDynamicNode = runningProcess->pJoiningProcesses.pHead;
            RemoveDoublyLinkedNode(&runningProcess->pJoiningProcesses,
                                   runningProcess->pJoiningProcesses.pHead);
            pProcessINeedToJoin = (Process *)pDynamicNode->pData;
            // set the status of the process to ready
            ChangeProcessStatus(priorityListQueue,
                                FindDoublyLinkedNode(pProcessINeedToJoin,
                                                     &priorityListQueue[STATUS_BLOCKED_JOIN]),
                                STATUS_READY);
            // free the linked list node
            DestroyDoublyLinkedNode(pDynamicNode);

            pProcessINeedToJoin->joinStatus = runningProcess->exitCode;
        }
    }

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
        // process has no children and no parent, clean up after self
        context_stop(runningProcess->context);
        InitializeProcessToDefault(runningProcess);
        InitializeDoublyLinkedNode(pStaticListNode);
    }

    runningProcess = NULL;
    dispatcher();
}

int k_kill(int pid, int signal)
{
    enforceKernelMode();
    int result = 0;
    Process *pProcess = NULL;
    DoublyLinkedNode *pListNode = NULL;

    disableInterrupts();
    // look for the process in the process table
    pListNode = FindStaticStorageNode(pid, staticNodeStorage);
    pProcess = (Process *)pListNode->pData;

    // if the process is not found or signal is not equal to SIG_TERM
    if (pListNode == NULL ||
        signal != SIG_TERM ||
        (pListNode != NULL && pProcess == NULL))
    {
        stop(1);
    }

    // check if the process is already signaled
    if (pProcess->signal)
    {
        return 1;
    }

    // set the signal for the process
    pProcess->signal = signal;
    enableInterrupts();
    return 0;
}

int k_getpid()
{
    enforceKernelMode();
    return runningProcess ? runningProcess->pid : 0;
}

int k_join(int pid, int *pChildExitCode)
{

    enforceKernelMode();
    disableInterrupts();
    int result = 0;

    // get the process from the process table
    Process *pProcess = NULL;
    DoublyLinkedNode *pNewJoiningProcessNode = NULL;
    DoublyLinkedNode *pStaticListNode = FindStaticStorageNode(pid, staticNodeStorage);

    if (pStaticListNode != NULL)
    {
        pProcess = (Process *)pStaticListNode->pData;
    }
    else
    {
        console_output(debugFlag, "join: attempting to join a process that does not exist.\n");
        stop(1);
    }

    // ensure that the process is not trying to join itself
    if (pProcess->pid == runningProcess->pid)
    {
        console_output(debugFlag, "join: process attempted to join itself.\n");
        stop(1);
    }

    // ensure the process is not trying to join its parent
    if (pid == runningProcess->pParent->pid)
    {
        console_output(debugFlag, "join: process attempted to join parent.\n");
        stop(2);
    }

    // create a new linked list node of the running process and add it to the joining processes list
    // of the process we are trying to join
    pNewJoiningProcessNode = CreateDoublyLinkedNode(runningProcess);

    // if the linked list node is NULL, return an error
    if (pNewJoiningProcessNode == NULL)
    {
        console_output(debugFlag,
                       "join(): Error: Could not create a new linked list node for the joining process.\n");
        return -6;
    }

    // add the running process to the joining processes list of the process we are trying to join
    InsertDoublyLinkedNode(&pProcess->pJoiningProcesses, pNewJoiningProcessNode);

    // set the status of the running process to blocked join
    ChangeProcessStatus(priorityListQueue,
                        FindDoublyLinkedNode(runningProcess,
                                             &priorityListQueue[STATUS_RUNNING]),
                        STATUS_BLOCKED_JOIN);

    // verify that our node made it the pJoiningProcesses list
    if (FindDoublyLinkedNode(runningProcess, &pProcess->pJoiningProcesses) == NULL)
    {
        return -1;
    }

    // call dispatcher to switch to the next process
    dispatcher();

    // _______________AFTER PROCESS WAS AWAKENED BY PROCESS THEY WANT TO JOIN____________________

    disableInterrupts();

    // look for the exit code of the process we want to join in the process table
    *pChildExitCode = runningProcess->joinStatus;

    // resetting the joinStatus to a known init value
    runningProcess->joinStatus = -99;

    dispatcher();

    return result;
}

int unblock(int pid)
{
    int isBlocked = 0;
    disableInterrupts();
    // get the process from the process table
    Process *pProcess = NULL;
    DoublyLinkedNode *pListNode = FindStaticStorageNode(pid, staticNodeStorage);

    if (pListNode != NULL)
    {
        pProcess = (Process *)pListNode->pData;
        isBlocked = pProcess->status == STATUS_BLOCKED_WAIT || pProcess->status == STATUS_BLOCKED_IO ||
                    (pProcess->status > NUM_PROCESS_STATES - 1);
    }

    // if invalid or process is not blocked, return -1
    if (pListNode == NULL || !isBlocked)
    {
        return -1;
    }

    ChangeProcessStatus(priorityListQueue, pListNode, STATUS_READY);

    dispatcher();
    return 0;
}

int block(int newStatus)
{

    // function blocks the calling process and sets the status in the process table to the value specified by newStatus

    // if the new status is between 0-10
    if (newStatus < 0 || newStatus < 10)
    {
        console_output(debugFlag, "block: function called with a reserved status value.\n");
        stop(1);
    }

    disableInterrupts();
    ChangeProcessStatus(priorityListQueue,
                        FindDoublyLinkedNode(runningProcess,
                                             &priorityListQueue[STATUS_RUNNING]),
                        newStatus);

    dispatcher();

    if (runningProcess->signal)
    {
        return -5;
    }

    return 0;
}

int signaled()
{
    return (runningProcess && runningProcess->signal) ? 1 : 0;
}

int read_time()
{
    return cpu_time(); // ? Not sure if this is correct, read_time is not is the spec
}

int cpu_time()
{
    return runningProcess ? runningProcess->cpuTime : 0;
}

DWORD read_clock()
{
    return system_clock();
}

int get_start_time(void)
{
    return runningProcess->startTime;
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
        else if (pProcess->status == STATUS_BLOCKED_JOIN)
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

void dispatcher()
{

    Process *pNextReadyProcess = NULL;

    // if we are in bootstrap, we need to return
    if (isBooting)
    {
        return;
    }

    // determine if the currently running process has exceeded its quantum
    if (runningProcess != NULL &&
        (runningProcess->elapsedTime >= runningProcess->quantum))
    {
        time_slice();
    }
    else
    {
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
            return;
        }
    }
}

/**
 * @brief Watchdog function
 *
 * The watchdog keeps the system going when all other processes are blocked.
 * It can be used to detect when the system is shutting down as well as when a deadlock
 * arises
 *
 * @param dummy A pointer to the dummy variable - use a void pointer or NULL
 */
static int watchdog(void *dummy)
{

    Process *pNextReadyProc = NULL;
    DoublyLinkedNode *pDynamicNode = NULL;
    DoublyLinkedNode *pStaticListNode = NULL;
    DoublyLinkedNode *pNode = priorityListQueue[STATUS_READY].pHead;

    if (isBooting)
    {
        // We are still booting up, so we need to wait for the system to be ready
        return 0;
    }

    if (pNode == NULL || pNode->pData == NULL)
    {
        check_deadlock();
        console_output(FALSE, "Processes still exist.\n");
        stop(1);
    }

    return 0;
}

/* check to determine if deadlock has occurred... */
static void check_deadlock()
{
    int i = 0;
    Process *pCurrentProc = NULL;
    DoublyLinkedNode *pNextLNode = NULL;

    disableInterrupts();
    // determine why the watchdog was called
    while (1)
    {
        // check for deadlock
        // print out the number of ready processes

        // loop over the priority list queue
        for (i = 0; i < NUM_PROCESS_STATES; i++)
        {
            // get the head of the list
            pNextLNode = priorityListQueue[i].pHead;

            // if the head is not null, we have a process
            if (pNextLNode != NULL)
            {
                // get the process
                pCurrentProc = (Process *)pNextLNode->pData;

                // if the process is not null, we have a process
                if (pCurrentProc != NULL && pCurrentProc->status != STATUS_RUNNING)
                {
                    // print the process name
                    console_output(FALSE, "Current List is the %s list.\n", STATUS_STRINGS[i]);
                    console_output(FALSE, "Current process is:  %s .\n", pCurrentProc->name);
                }
            }
        }
        break;
    }

    console_output(FALSE, "All processes completed.\n");

    // we arrived here because there are no processes in the system
    stop(0);
}

/*
 * Disables the interrupts.
 */
static inline void disableInterrupts()
{
    /* We ARE in kernel mode */
    set_psr(get_psr() & ~PSR_INTERRUPTS);
}

/*
 * Enables the interrupts.
 */
static inline void enableInterrupts()
{
    set_psr(get_psr() | PSR_INTERRUPTS);
}

/**
 * @brief Debug console function
 *
 * Prints the message to the console_output if in debug mode
 *
 * @param format a pointer to the format string
 * @param ... a pointer to the va args
 *
 * @note This function is called when the debug flag is set
 */
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

void time_slice()
{
    disableInterrupts();
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
        enableInterrupts();
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
    disableInterrupts();
    if ((get_psr() & PSR_KERNEL_MODE) == 0)
    {
        console_output(debugFlag, "Kernel mode expected, but function called in user mode.\n");
        stop(1);
    }
    enableInterrupts();
}
