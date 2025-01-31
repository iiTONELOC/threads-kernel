#include "Processes.h"
#include "Constants.h"

/*_______________________Function  Definitions_______________________*/

int OrderFunction(void *pNode1, void *pNode2)
{
    Process *process1 = (Process *)((DoublyLinkedNode *)pNode1)->pData;
    Process *process2 = (Process *)((DoublyLinkedNode *)pNode2)->pData;

    if (process1 == NULL || process2 == NULL)
    {
        return 0;
    }

    // descending order, the linked list test runs ascending order
    // so here we cover both bases as this function is passed to the
    // linked list's initialization function
    return process2->priority - process1->priority;
}

void InitializeProcessToDefault(Process *usingProcessPtr)
{
    usingProcessPtr->pid = -1;
    usingProcessPtr->status = -1;
    usingProcessPtr->signal = -1;
    usingProcessPtr->quantum = 0; // not currently used by timeslice
    usingProcessPtr->cpuTime = 0;
    usingProcessPtr->stack = NULL;
    usingProcessPtr->exitCode = -1;
    usingProcessPtr->priority = -1;
    usingProcessPtr->startTime = -1;
    usingProcessPtr->stacksize = -1;
    usingProcessPtr->context = NULL;
    usingProcessPtr->pParent = NULL;
    usingProcessPtr->elapsedTime = -1;
    usingProcessPtr->entryPoint = NULL;
    usingProcessPtr->processTableIndex = -1;
    InitializeDoublyLinkedList(&usingProcessPtr->pChildren, NULL);
    InitializeDoublyLinkedList(&usingProcessPtr->pDeadChildren, NULL);
    InitializeDoublyLinkedList(&usingProcessPtr->pExitingChildren, NULL);
    InitializeDoublyLinkedList(&usingProcessPtr->pJoiningProcesses, NULL);
}

void InitializeProcessTable(Process *usingTablePtr, int size)
{
    int i = 0;
    // loop over the array of nodes and initialize each node to
    // NULL values
    for (i = 0; i < size; i++)
    {
        InitializeProcessToDefault(&usingTablePtr[i]);
    }
}

int GetEmptyControlBlockIndex(Process *fromProcessTablePtr)
{
    int i;

    for (i = 0; i < MAXPROC; i++)
    { // if a pid hasn't been assigned and the context is NULL
        // we have a slot we can use - regardless of whatever else is in the slot
        if (fromProcessTablePtr[i].context == NULL)
        {
            return i;
        }
    }

    return -1;
}

DoublyLinkedNode *FindProcessNodeByPid(int pid, DoublyLinkedNode *pNodeBucket)
{
    int i = 0;

    // loop over the array of nodes and look for a process with the
    // corresponding pid
    for (i = 0; i < MAXPROC; i++)
    {

        // If we have NULL data just skip to the next node
        if (((DoublyLinkedNode *)&pNodeBucket[i])->pData == NULL)
        {
            continue;
        }

        // Non-NULL data, check the pid
        if (((Process *)((DoublyLinkedNode *)&pNodeBucket[i])->pData)->pid == pid)
        {
            // match found
            return &pNodeBucket[i];
        }
    }

    // no match found
    return NULL;
}
