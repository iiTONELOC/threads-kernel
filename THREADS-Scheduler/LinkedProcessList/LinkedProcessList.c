#include "LinkedProcessList.h"

enum ListType LIST_TYPE;

void InitializeProcessList(LinkedProcessList *pList, enum ListType listType)
{
    pList->count = 0;
    pList->pHead = NULL;
    pList->pTail = NULL;
    pList->listType = listType;
}

Process *GetNextPtrForListType(enum ListType listType, Process *pProcess)
{
    Process *pNextPtr = NULL;

    switch (listType)
    {
    case READY_PROCESSES_LIST:
        pNextPtr = pProcess->pNextReadyProcess;
        break;
    case PROCESS_CHILDREN_LIST:
        pNextPtr = pProcess->pNextChild;
        break;
    case PROCESS_ZOMBIE_CHILDREN_LIST:
        pNextPtr = pProcess->pNextZombieChild;
        break;
    case PROCESS_EXITING_CHILDREN_LIST:
        pNextPtr = pProcess->pNextExitingChild;
        break;
    case PROCESS_JOINING_PROCESSES_LIST:
        pNextPtr = pProcess->pNextJoiner;
        break;
    default:
        break;
    }

    return pNextPtr;
}

Process *PopProcessFromList(LinkedProcessList *pList)
{
    Process *pProcess = NULL;
    Process *pNextPtr = NULL;

    if (pList->count > 0)
    {
        // Pop the process from the front of the list
        pProcess = pList->pHead;
        // Update the head pointer to the next process in the list
        pNextPtr = GetNextPtrForListType(pList->listType, pProcess);
        pList->pHead = pNextPtr;
        // Adjust list count
        pList->count--;

        // If the list is now empty, update the tail pointer
        if (pList->count == 0)
        {
            pList->pHead = pList->pTail = NULL;
        }

        // Clear the next pointer for the popped process
        if (pNextPtr != NULL)
        {
            pNextPtr = NULL;
        }
    }

    return pProcess;
}

void AddProcessToList(Process *pProcess, LinkedProcessList *pList)
{
    // Get the next pointer for the process we want to ad
    Process *pNextPtr = GetNextPtrForListType(pList->listType, pProcess);

    // Ensure the next pointer is null, we are appending to the end of the list
    if (pNextPtr != NULL)
    {
        pNextPtr = NULL;
    }

    // If the head is null, the list is empty
    if (pList->pHead == NULL)
    {
        pList->pHead = pList->pTail = pProcess;
    }
    else
    {
        // Update the next pointer for the current tail process
        pNextPtr = GetNextPtrForListType(pList->listType, pList->pTail);
        pNextPtr = pProcess;
        // Update the tail pointer
        pList->pTail = pProcess;
    }

    // Adjust list count
    pList->count++;
}

void RemoveProcessFromList(LinkedProcessList *pList, Process *pProcess)
{
    Process *pTemp = NULL;
    Process *pCurrent = NULL;
    Process *pPrevProcess = NULL;
    Process *pNextProcess = NULL;

    // check if the pProcess is the head of the list
    if (pList->pHead == pProcess)
    {
        // remove the current head by setting the head to the next process
        pList->pHead = GetNextPtrForListType(pList->listType, pProcess);
    }

    // traverse the list to find the process to remove, even if its at the tail we want the previous process

    pTemp = pList->pHead;

    while (pTemp != NULL)
    {
        if (pTemp == pProcess)
        {
            pCurrent = pTemp;
            break;
        }
        pPrevProcess = pTemp;
        pTemp = GetNextPtrForListType(pList->listType, pTemp);
    }

    // check if the pProcess is the tail of the list
    if (pList->pTail == pProcess)
    {
        // remove the current tail by setting the tail to the previous process
        pList->pTail = pPrevProcess;

        // update the next pointer for the new tail to NULL
        pNextProcess = GetNextPtrForListType(pList->listType, pList->pTail);
        pNextProcess = NULL;
    }
    else
    {
        // remove the process from the list by updating the next pointer
        // for the previous process to the next process of the process to remove
        pNextProcess = GetNextPtrForListType(pList->listType, pCurrent);
        pPrevProcess = pNextProcess;
    }

    // set the next pointer for the removed process to NULL
    pTemp = NULL;

    // Adjust list count
    pList->count--;

    // If the list is now empty, update the head and tail pointers
    if (pList->count == 0)
    {
        pList->pHead = pList->pTail = NULL;
    }

    // Clear the next pointer for the removed process
    pTemp = GetNextPtrForListType(pList->listType, pCurrent);
    pTemp = NULL;
}
