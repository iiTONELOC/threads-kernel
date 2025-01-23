#ifndef PROCESSES_H
#include "Processes.h"
#endif

/*_______________________Global Variables_______________________*/

static Process processTable[MAX_PROCESSES];           // storage table for all processes
static Process processStatusList[NUM_PROCESS_STATES]; // process state list for ready, running, blocked, quit

/*_______________________Function  Definitions_______________________*/

/**
 * @brief Initializes a process to NULL
 *
 * @param usingProcessPtr The node to initialize
 */
void initializeProcessToNull(Process *usingProcessPtr)
{
    usingProcessPtr->pid = NULL;
    usingProcessPtr->stack = NULL;
    usingProcessPtr->status = NULL;
    usingProcessPtr->signal = NULL;
    usingProcessPtr->endTime = NULL;
    usingProcessPtr->name[0] = NULL;
    usingProcessPtr->context = NULL;
    usingProcessPtr->pParent = NULL;
    usingProcessPtr->exitCode = NULL;
    usingProcessPtr->waitTime = NULL;
    usingProcessPtr->priority = NULL;
    usingProcessPtr->blockTime = NULL;
    usingProcessPtr->startTime = NULL;
    usingProcessPtr->stacksize = NULL;
    usingProcessPtr->timeSlice = NULL;
    usingProcessPtr->pChildren = NULL;
    usingProcessPtr->entryPoint = NULL;
    usingProcessPtr->tableIndex = NULL;
    usingProcessPtr->elapsedTime = NULL;
    usingProcessPtr->startArgs[0] = NULL;
    usingProcessPtr->demotionTime = NULL;
    usingProcessPtr->demotionCount = NULL;
    usingProcessPtr->nextSiblingProcess = NULL;
    for (int i = 0; i < 4; i++)
    {
        usingProcessPtr->nextProcessPtr[i] = NULL;
    }
}

/**
 * @brief Initializes the processes Table
 *
 * Takes an array of Processes and initializes all values to NULL
 *
 * @param usingTablePtr A pointer to the process table to initialize
 * @param withNextProcessTbl A pointer to the next process table
 */
void initializeProcessTable(Process *usingTablePtr)
{
    // loop over the array of nodes and initialize each node to
    // NULL values
    for (int i = 0; i < MAXPROC; i++)
    {
        initializeProcessToNull(&usingTablePtr[i]);
    }
}

/**
 * @brief Retrieve the next empty process slot from the proccess table
 *
 * @param fromProcessTablePtr Pointer to the process table
 *
 * @return The index into the process table or -1 if the table is full
 */
int getEmptyControlBlockIndex(Process *fromProcessTablePtr)
{
    int i;

    for (i = 0; i < MAXPROC; i++)
    {
        if (fromProcessTablePtr[i].pid == NULL && fromProcessTablePtr[i].context == NULL)
            return i;
    }

    return -1;
}

/**
 * @brief Add a child process to the parent process
 *
 * @param childPtr Pointer to the process to add as a child
 * @param toParentPtr Pointer to the parent process
 *
 * @note The process is added to the end of the list
 */
void addChildProcess(Process *childPtr, Process *toParentPtr)
{
    Process *listHeadPtr = &toParentPtr->pChildren[0];
    // if the head is NULL then set the head to the new process
    if (listHeadPtr == NULL)
    {
        toParentPtr->pChildren = childPtr;
        return;
    }

    // get the next empty sibling
    Process *next = listHeadPtr;
    while (next->nextSiblingProcess != NULL)
    {
        next = next->nextSiblingProcess;
    }

    // add the new process to the end of the list
    next->nextSiblingProcess = childPtr;

    // set the parent
    childPtr->pParent = toParentPtr;
}

/**
 * @brief Remove a child process from the parent process
 *
 * @param childPtr Pointer to the process to remove
 * @param fromParentPtr Pointer to the head of the list of children
 */
void removeChildProcess(Process *childPtr, Process *fromParentPtr)
{
    Process **listHeadPtr = &fromParentPtr->pChildren;
    // if the head is NULL then return
    if (*listHeadPtr == NULL)
    {
        return;
    }

    // if the head is the process to remove
    if (*listHeadPtr == childPtr)
    {
        *listHeadPtr = childPtr->nextSiblingProcess;
        return;
    }

    // get the next empty sibling
    Process *next = *listHeadPtr;
    while (next->nextSiblingProcess != childPtr)
    {
        next = next->nextSiblingProcess;
    }

    // remove the process from the list
    next->nextSiblingProcess = childPtr->nextSiblingProcess;

    // set the parent to NULL
    childPtr->pParent = NULL;

    // set the next sibling to NULL
    childPtr->nextSiblingProcess = NULL;

    // set the status to QUIT
    childPtr->status = QUIT;
}

/**
 * @brief Insert a process into the process table at the specified index.
 *
 * @param newProcessPtr Pointer to the process to add to the table
 * @param usingTablePtr Pointer to the proccess table to add the new process
 * @param runningProcess Pointer to the process that is currently running
 * @param atTableIndex Index in the table, set to -1 to fetch the next available slot
 */
void insertIntoProcessTable(Process *newProcessPtr,
                            Process *usingTablePtr,
                            Process **runningProcess,
                            int atTableIndex)
{
    // allow an 'optional' index param
    // if it is set to -1, get the next slot
    if (atTableIndex == -1)
    {
        atTableIndex = getEmptyControlBlockIndex(usingTablePtr);
    }

    // no space is available in the table
    if (atTableIndex == -1)
    {
        return;
    }

    // copy the new process into the table
    usingTablePtr[atTableIndex] = *newProcessPtr;

    // set the table index for the process
    usingTablePtr[atTableIndex].tableIndex = atTableIndex;

    // check if a process is currently running
    if (*runningProcess != NULL)
    {
        console_output(1, "Table Insertion - A Parent Process, The: %s is running\n", (*runningProcess)->name);

        // add the new process to the parent's children list
        addChildProcess(newProcessPtr, *runningProcess);
    }
}

/**
 * @brief Remove a process from the process table.
 *
 * @param processNodePtr Pointer to the process to remove from the table
 * @param usingTablePtr Pointer to the process table
 * @param runningProcess Pointer to the process that is currently running
 */
void removeFromProcessTable(Process *processNodePtr, Process *usingTablePtr,
                            Process *runningProcess, Process *processList)
{
    // remove the process from the table if and only if it has no children
    if (processNodePtr->pChildren == NULL)
    {
        // remove the process from the parent's children list
        removeChildProcess(processNodePtr, processNodePtr);
        // remove the process from the table
        initializeProcessToNull(&usingTablePtr[processNodePtr->tableIndex]);
    }
}

/**
 * @brief Set up a process
 *
 * Configures a new process with the specified values
 * @param newProcessPtr Pointer to the process to set up
 * @param pid The process id
 * @param tblIndex The index in the process table
 * @param name The process name
 * @param entryPoint The entry point for the process
 * @param arg The arguments for the process
 * @param stacksize The stack size for the process
 * @param priority The priority for the process
 *
 * @note The timing information is set to an initialized 0 value, the status
 *  is set to READY and the time slice is set to the default time slice time
 *  of 20ms.
 */
void configureProcessForTable(Process *newProcessPtr, int pid, int tblIndex, char *name,
                              int (*entryPoint)(void *), void *arg, int stacksize,
                              int priority)
{
    newProcessPtr->pid = pid;                         // set process id
    newProcessPtr->signal = 0;                        // set the signal to an initial value
    newProcessPtr->endTime = 0;                       // set the end time to an initial value
    newProcessPtr->waitTime = 0;                      // set the wait time to an initial value
    newProcessPtr->startTime = 0;                     // set the start time to an initial value
    newProcessPtr->blockTime = 0;                     // set the block time to an initial value
    newProcessPtr->status = READY;                    // set the status to ready
    newProcessPtr->elapsedTime = 0;                   // set the elapsed time to an initial value
    *newProcessPtr->startArgs = arg;                  // set process arguments
    strcpy(newProcessPtr->name, name);                // set process name
    newProcessPtr->priority = priority;               // set process priority
    newProcessPtr->stacksize = stacksize;             // set process stack size
    newProcessPtr->tableIndex = tblIndex;             // set the table index
    newProcessPtr->entryPoint = entryPoint;           // set process entry point
    newProcessPtr->timeSlice = DEFAULT_TIME_SLICE_MS; // set the time slice
}

/**
 * @brief Initializes the process list
 *
 * Takes a pointer to a ProcessLists struct and Process Table and initializes the
 * remaining pointers in the ProcessList struct to NULL
 *
 * @param usingProcessListPtr A pointer to the process lists to initialize
 */
void initializeProcessList(Process **usingProcessListPtr)
{
    // loop over the list of processes and set each pointer to NULL
    for (int i = 0; i < 4; i++)
    {
        usingProcessListPtr[i] = NULL;
    }
}

/**
 * @brief Insert a process into the linked list of processes at the specified table index.
 *
 *
 * @param newProcessNodePtr Pointer to the process node to add to the list
 * @param usingStatusListPtr Pointer to the status list
 * @param withStatus Status of the process
 * @param withPriority Priority of the process, this is an integer value from 0 to 5
 *
 *
 * @note Constants available in `SchedulerConstants.h` can be used for their respective indexes
 *
 * ```c
 *  #define READY 0
 *  #define RUNNING 1
 *  #define BLOCKED 2
 *  #define QUIT 3
 *
 * #define LOWEST_PRIORITY 0
 * #define HIGHEST_PRIORITY 5
 *
 * #define PRIORITY_LEVEL_0 0
 * #define PRIORITY_LEVEL_1 1
 * #define PRIORITY_LEVEL_2 2
 * #define PRIORITY_LEVEL_3 3
 * #define PRIORITY_LEVEL_4 4
 * #define PRIORITY_LEVEL_5 5
 * ```
 * @example
 * ```c
 * insertIntoProcessList(newProcessNodePtr, &processList.headReadyProcessesPtr, READY, READY, 3);
 * ```
 *
 */
void insertIntoProcessList(Process *newProcessNodePtr,
                           Process *usingStatusListPtr,
                           int withStatus,
                           int withPriority)
{
    Process *previousProcessPtr = NULL;
    Process *currentProcessPtr = &usingStatusListPtr[withStatus];

    // debug this process
    console_output(1, "Inserting Process %s into the %s list\n",
                   newProcessNodePtr->name, usingStatusListPtr[withStatus].name);

    // TODO: FIX THIS BROKEN AZZ CHIT - IF an entry is in the list its overwriting it
    //  rather than adding it to the end

    //  loop over the list to find the correct position to insert the new process
    //  based on the priority (larger priorities at the beginning)
    while (currentProcessPtr != NULL && currentProcessPtr->priority > withPriority)
    {
        previousProcessPtr = currentProcessPtr;
        currentProcessPtr = currentProcessPtr->nextProcessPtr[withStatus];
    }

    // if the previous process is NULL then the new process is the head
    if (previousProcessPtr == NULL)
    {
        usingStatusListPtr[withStatus] = *newProcessNodePtr;
    }
    else
    {
        // insert the new process between the previous and current process
        previousProcessPtr->nextProcessPtr[withStatus] = newProcessNodePtr;
        // update the next pointer of the new process to the current process
        newProcessNodePtr->nextProcessPtr[withStatus] = currentProcessPtr;
    }

    // set the next pointer of the new node to the current node
    newProcessNodePtr->nextProcessPtr[withStatus] = currentProcessPtr;

    // set the status of the new process if it is different from the current status
    if (newProcessNodePtr->status != withStatus)
    {
        newProcessNodePtr->status = withStatus;
    }

    // set the priority of the new process if it is different from the current priority
    if (newProcessNodePtr->priority != withPriority)
    {
        newProcessNodePtr->priority = withPriority;
    }
}

/**
 * @brief Remove a process from the linked list of processes
 *
 * @param processNodePtr Pointer to the process node to remove from the list
 * @param usingProcessListHeadPtr Pointer to a pointer to the head of the process list
 *
 * @example
 * ```c
 * removeFromProcessList(processNodePtr, &processList.headReadyProcessesPtr);
 * ```
 */
void removeFromProcessList(Process *usingProcessPtr, Process *usingProcessListHeadPtr)
{
    Process *previousProcessPtr = NULL;

    // loop over the list to find the process to remove
    while (usingProcessListHeadPtr != NULL && usingProcessListHeadPtr != usingProcessPtr)
    {
        previousProcessPtr = usingProcessListHeadPtr;
        usingProcessListHeadPtr = usingProcessListHeadPtr->nextProcessPtr[usingProcessPtr->status];
    }

    // if the current process is NULL then the process was not found
    if (usingProcessListHeadPtr == NULL)
    {
        return;
    }

    // if the previous process is NULL then the process to remove is the head
    if (previousProcessPtr == NULL)
    {
        usingProcessListHeadPtr = usingProcessPtr->nextProcessPtr[usingProcessPtr->status];
    }
    else
    {
        // otherwise set the next pointer of the previous process to the next pointer
        // of the current process to remove it
        previousProcessPtr->nextProcessPtr[usingProcessPtr->status] =
            usingProcessPtr->nextProcessPtr[usingProcessPtr->status];
    }

    // set the next pointer of the using process to NULL
    usingProcessPtr->nextProcessPtr[usingProcessPtr->status] = NULL;

    // set the status of the process to NULL
    usingProcessPtr->status = NULL;
}
