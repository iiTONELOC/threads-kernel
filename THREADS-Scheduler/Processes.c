#include "SchedulerConstants.h"
#include "Processes.h"

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
    usingProcessPtr->name[0] = NULL;
    usingProcessPtr->context = NULL;
    usingProcessPtr->pParent = NULL;
    usingProcessPtr->priority = NULL;
    usingProcessPtr->stacksize = NULL;
    usingProcessPtr->pChildren = NULL;
    usingProcessPtr->entryPoint = NULL;
    usingProcessPtr->tableIndex = NULL;
    usingProcessPtr->startArgs[0] = NULL;
    usingProcessPtr->nextProcessPtr[0] = NULL;
    usingProcessPtr->nextSiblingProcess = NULL;
}

/**
 * @brief Initializes the processes Table
 *
 * Takes an array of Processes and initializes all values to NULL
 *
 * @param usingTablePtr A pointer to the process table to initialize
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
 * @param usingProcessPtr Pointer to the process to add as a child
 * @param withListHeadPtr Pointer to the head of the list of children
 *
 * @note The process is added to the end of the list
 */
void addChildProcess(Process *usingProcessPtr, Process **withListHeadPtr)
{
    // if the head is NULL then set the head to the new process
    if (*withListHeadPtr == NULL)
    {
        *withListHeadPtr = usingProcessPtr;
        return;
    }

    // get the next empty sibling
    Process *next = *withListHeadPtr;
    while (next->nextSiblingProcess != NULL)
    {
        next = next->nextSiblingProcess;
    }
}

/**
 * @brief Remove a child process from the parent process
 *
 * @param usingProcessPtr Pointer to the process to remove
 * @param withListHeadPtr Pointer to the head of the list of children
 */
void removeChildProcess(Process *usingProcessPtr, Process **withListHeadPtr)
{
    // if the head is NULL then return
    if (*withListHeadPtr == NULL)
    {
        return;
    }

    // if the head is the process to remove
    if (*withListHeadPtr == usingProcessPtr)
    {
        *withListHeadPtr = usingProcessPtr->nextSiblingProcess;
        return;
    }

    // get the next empty sibling
    Process *next = *withListHeadPtr;
    while (next->nextSiblingProcess != usingProcessPtr)
    {
        next = next->nextSiblingProcess;
    }

    // remove the process from the list
    next->nextSiblingProcess = usingProcessPtr->nextSiblingProcess;
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
                            Process *runningProcess,
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
    if (runningProcess != NULL)
    {
        // add the new process to the parent's children list
        addChildProcess(&usingTablePtr[atTableIndex], &runningProcess->pChildren);

        // set the parent for this process
        usingTablePtr[atTableIndex].pParent = runningProcess;
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
                            Process *runningProcess)
{
    // remove the process from the table if and only if it has no children
    if (processNodePtr->pChildren == NULL)
    {
        // remove the process from the parent's children list
        removeChildProcess(processNodePtr, &processNodePtr->pParent->pChildren);
        // remove the process from the table
        initializeProcessToNull(&usingTablePtr[processNodePtr->tableIndex]);
    }
}