#include "PriorityProcessQueue.h"

// __________________________ Constants __________________________

const char *STATUS_STRINGS[NUM_PROCESS_STATES] = {
    "READY",
    "RUNNING",
    "BLOCKED_WAIT",
    "BLOCKED_JOIN",
    "BLOCKED_IO",
    "QUIT",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "USER_DEFINED"};
// __________________________ Function Definitions __________________________

int GetStatusListIndex(int status)
{
    int safeStatus = status;

    if (safeStatus < 0 || safeStatus > NUM_PROCESS_STATES)
    {
        safeStatus = STATUS_USER_DEFINED;
    }

    return safeStatus;
}
void InitializePriorityProcessQueueArray(DoublyLinkedList *usingArrayPtr, int numStates)
{
    int i = 0;

    for (i = 0; i < numStates; i++)
    {
        InitializeDoublyLinkedList(&usingArrayPtr[i], OrderFunction);
    }
}

void AddNodeToPriorityProcessQueue(DoublyLinkedList *usingQueuePtr, DoublyLinkedNode *pListNode)
{
    if (usingQueuePtr == NULL || pListNode == NULL)
    {
        return;
    }

    InsertDoublyLinkedNode(usingQueuePtr, pListNode);
}

void RemoveNodeFromPriorityProcessQueue(DoublyLinkedList *usingListPtr, DoublyLinkedNode *pListNode)
{
    if (usingListPtr == NULL || pListNode == NULL)
    {
        return;
    }

    RemoveDoublyLinkedNode(usingListPtr, pListNode);
}

void ChangeProcessStatus(DoublyLinkedList *usingListPtr, DoublyLinkedNode *pListNode, int newStatus)
{
    int safeIndex = -1;

    if (usingListPtr == NULL || pListNode == NULL)
    {
        return;
    }

    if (pListNode->pData == NULL)
    {
        return;
    }

    if (((Process *)pListNode->pData)->status == newStatus)
    {
        return;
    }

    safeIndex = GetStatusListIndex(newStatus);

    RemoveNodeFromPriorityProcessQueue(&usingListPtr[GetStatusListIndex(((Process *)pListNode->pData)->status)], pListNode);
    AddNodeToPriorityProcessQueue(&usingListPtr[safeIndex], pListNode);
    ((Process *)pListNode->pData)->status = newStatus;
}

DoublyLinkedNode *FindStaticStorageNode(int withPid, DoublyLinkedNode *pNodeBucket)
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
        if (((Process *)((DoublyLinkedNode *)&pNodeBucket[i])->pData)->pid == withPid)
        {
            // match found
            return &pNodeBucket[i];
        }
    }

    // no match found
    return NULL;
}
