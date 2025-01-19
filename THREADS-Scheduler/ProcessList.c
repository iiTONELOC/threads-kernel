/**
 * Adapted from
 * CYBV470 - Extra Credit Assignment
 * Student: Anthony Tropeano
 * Date: 12/6/2024
 */
#include "ProcessList.h"

/*_______________________Function  Definitions_______________________*/

/**
 * @brief Initializes the process list
 *
 * Takes a pointer to a ProcessLists struct and Process Table and initializes the
 * remaining pointers in the ProcessList struct to NULL
 *
 * @param usingProcessListPtr A pointer to the process lists to initialize
 */
void initializeProcessList(ProcessList *usingProcessListPtr)
{
    // initialize the pointers to NULL
    usingProcessListPtr->headReadyProcessesPtr = NULL;
    usingProcessListPtr->headRunningProcessesPtr = NULL;
    usingProcessListPtr->headBlockedProcessesPtr = NULL;
}

/**
 * @brief Insert a process into the linked list of processes at the specified table index.
 *
 *
 * @param newProcessNodePtr Pointer to the process node to add to the list
 * @param usingProcessListHeadPtr Pointer to a pointer to the head of the process list
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
                           Process **usingProcessListHeadPtr,
                           int withStatus,
                           int withPriority)
{
    Process *previousProcessPtr = NULL;
    Process *currentProcessPtr = *usingProcessListHeadPtr;

    // loop over the list to find the correct position to insert the new process
    // based on the priority
    while (currentProcessPtr != NULL && currentProcessPtr->priority < withPriority)
    {
        previousProcessPtr = currentProcessPtr;
        currentProcessPtr = currentProcessPtr->nextProcessPtr[withStatus];
    }

    // if the previous process is NULL then the new process is the head
    if (previousProcessPtr == NULL)
    {
        *usingProcessListHeadPtr = newProcessNodePtr;
    }
    else
    {
        // insert the new process between the previous and current process
        previousProcessPtr->nextProcessPtr[withStatus] = newProcessNodePtr;
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
void removeFromProcessList(Process *usingProcessPtr, Process **usingProcessListHeadPtr)
{
    Process *previousProcessPtr = NULL;
    Process *currentProcessPtr = *usingProcessListHeadPtr;

    // loop over the list to find the process to remove
    while (currentProcessPtr != NULL && currentProcessPtr != usingProcessPtr)
    {
        previousProcessPtr = currentProcessPtr;
        currentProcessPtr = currentProcessPtr->nextProcessPtr[usingProcessPtr->status];
    }

    // if the current process is NULL then the process was not found
    if (currentProcessPtr == NULL)
    {
        return;
    }

    // if the previous process is NULL then the process to remove is the head
    if (previousProcessPtr == NULL)
    {
        *usingProcessListHeadPtr = usingProcessPtr->nextProcessPtr[usingProcessPtr->status];
    }
    else
    {
        // remove the process from the list
        previousProcessPtr->nextProcessPtr[usingProcessPtr->status] = usingProcessPtr->nextProcessPtr[usingProcessPtr->status];
    }

    // set the next pointer of the process to NULL
    usingProcessPtr->nextProcessPtr[usingProcessPtr->status] = NULL;
}
