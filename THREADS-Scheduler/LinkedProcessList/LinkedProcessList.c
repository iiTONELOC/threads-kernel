#include "LinkedProcessList.h"

enum ListType LIST_TYPE;

/**
 * @brief Get the index for the list type
 * Returns the adjusted index for the list type and the process's master list.
 * There are 6 list types, but a process only manages 4 of them. Valid list types
 * start with PROCESS_ and are in the range of 2 to 5.
 *
 * @param listType The type of list to get the index for
 * @return int The index for the list type or -1 if the list type is invalid
 *
 * `Example Usage`
 * ```c
 * int result = GetProcessListIndex(PROCESS_CHILDREN_LIST);
 * // result == 0
 *
 * int result = GetProcessListIndex(READY_PROCESSES_LIST);
 * // result == -1
 * ```
 */
int GetProcessListIndex(enum ListType listType)
{
    if (listType >= LIST_TYPE_TO_PROC_MASTER_OFFSET && listType <= MAX_LIST_TYPES)
    {
        return listType - LIST_TYPE_TO_PROC_MASTER_OFFSET;
    }
    return -1;
}

/**
 * @brief Initialize the linked list
 *
 * @param pList  Pointer to the linked list to initialize
 * @param listType  The type of list to initialize
 * @return void
 */
void InitializeProcessList(LinkedProcessList *pList, enum ListType listType)
{
    pList->count = 0;
    pList->pHead = NULL;
    pList->pTail = NULL;
    pList->listType = listType;
}

/**
 * @brief Get the next pointer for the list type
 *
 * @param listType  The type of list to get the next pointer for
 * @param pProcess  Pointer to the process to get the next pointer for
 * @return void
 */
Process *GetNextPtrForList(enum ListType listType, Process *pProcess)
{
    // validate the input
    if (pProcess == NULL ||
        listType == UNINITIALIZED_LIST ||
        listType > MAX_LIST_TYPES)
    {
        return NULL;
    }

    // return the correct pointer from the process for the list
    // we want to interact with
    switch (listType)
    {
    case READY_PROCESSES_LIST:
        return pProcess->pNextReadyProcess;
    case PROCESS_CHILDREN_LIST:
        return pProcess->pNextChild;
    case PROCESS_ZOMBIE_CHILDREN_LIST:
        return pProcess->pNextZombieChild;
    case PROCESS_EXITING_CHILDREN_LIST:
        return pProcess->pNextExitingChild;
    case PROCESS_JOINING_PROCESSES_LIST:
        return pProcess->pNextJoiner;
    default:
        return NULL;
    }
}

/**
 * @brief Pop a process from the front of the linked list
 *
 * @param pList  Pointer to the linked list to pop the process from
 * @return Process*  Pointer to the process that was popped from the list
 */
Process *PopProcessFromList(LinkedProcessList *pList)
{
    Process *pProcess = pList->pHead; // Get the head of the list

    RemoveProcessFromList(pList, pList->pHead); // Remove it

    return pProcess;
}

/**
 * @brief Add a process to the end of the linked list
 *
 * @param pProcess  Pointer to the process to add to the list
 * @param pList  Pointer to the linked list to add the process to
 * @return void
 */
void AddProcessToList(Process *pProcess, LinkedProcessList *pList)
{
    // if the list is empty, set the head and tail to the new process
    if (pList->count == 0)
    {
        pList->pHead = pList->pTail = pProcess;
        // set the next pointer for the new process to NULL
        Process *ppNextPtr = GetNextPtrForList(pList->listType, pProcess);
        if (ppNextPtr != NULL)
        {
            ppNextPtr = NULL;
        }
    }
    else
    {

        // Get the next pointer for the tail process
        Process *ppNextPtr = GetNextPtrForList(pList->listType, pList->pTail);

        // set the current tail's next pointer to the new process
        if (ppNextPtr != NULL)
        {
            ppNextPtr = pProcess;
        }

        // Update the tail pointer to the new process
        pList->pTail = pProcess;
    }

    // Clear the next pointer for the new process
    Process *ppNextPtr = GetNextPtrForList(pList->listType, pProcess);
    if (ppNextPtr != NULL)
    {
        ppNextPtr = NULL;
    }
    // Adjust list count
    pList->count++;
}

/**
 * @brief Remove a process from the linked list
 *
 * @param pList  Pointer to the linked list to remove the process from
 * @param pProcess  Pointer to the process to remove from the list
 * @return void
 */
void RemoveProcessFromList(LinkedProcessList *pList, Process *pProcess)
{
    Process *pTemp = NULL;         // General pointer for traversing the list
    Process *pPrevProcess = NULL;  // Pointer to the previous process of the process to remove
    Process *ppNextProcess = NULL; // Pointer to the next process of the process to remove

    // Check if pProcess is the head of the list
    if (pList->pHead == pProcess)
    {
        // Remove the head by setting the head to the next process
        ppNextProcess = GetNextPtrForList(pList->listType, pProcess);
        pList->pHead = (ppNextProcess != NULL) ? ppNextProcess : NULL;
    }

    // Traverse the list to find the process to remove
    // (keeps track of the previous process)
    pTemp = pList->pHead;
    while (pTemp != NULL)
    {
        if (pTemp == pProcess)
        {
            // Found the process to remove
            break;
        }
        // Update the previous process
        pPrevProcess = pTemp;
        // Get the next process in the list
        ppNextProcess = GetNextPtrForList(pList->listType, pTemp);
        // Move to the next process
        pTemp = (ppNextProcess != NULL) ? ppNextProcess : NULL;
    }

    // Check if pProcess is the tail of the list
    if (pList->pTail == pProcess)
    {
        // Update the tail pointer to the previous process
        pList->pTail = pPrevProcess;

        // Ensure the new tail's next pointer is NULL
        if (pPrevProcess != NULL)
        {
            ppNextProcess = GetNextPtrForList(pList->listType, pList->pTail);
            if (ppNextProcess != NULL)
            {
                ppNextProcess = NULL;
            }
        }
    }
    else if (pPrevProcess != NULL) // not head, not tail
    {
        // Remove the process by updating the previous process's next pointer
        // to point to the current process's next pointer
        ppNextProcess = GetNextPtrForList(pList->listType, pProcess);
        Process *pNextProcess = (ppNextProcess != NULL) ? ppNextProcess : NULL;

        // Update the previous process to skip the process to remove
        ppNextProcess = GetNextPtrForList(pList->listType, pPrevProcess);
        if (ppNextProcess != NULL)
        {
            ppNextProcess = pNextProcess;
        }
    }

    // Adjust list count
    pList->count--;

    // If the list is empty, reset head and tail
    if (pList->count == 0)
    {
        pList->pHead = pList->pTail = NULL;
    }

    // if there is only one node left
    // ensure the head and tail are correct
    if (pList->count == 1)
    {
        // if the head is null
        if (pList->pHead == NULL)
        {
            pList->pHead = pList->pTail;
        }
        // if the tail is null
        if (pList->pTail == NULL)
        {
            pList->pTail = pList->pHead;
        }
    }

    // Clear the next pointer for the removed process
    ppNextProcess = GetNextPtrForList(pList->listType, pProcess);
    if (ppNextProcess != NULL)
    {
        ppNextProcess = NULL;
    }
}
