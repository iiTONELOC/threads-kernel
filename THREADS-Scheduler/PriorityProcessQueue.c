
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
    if (usingListPtr == NULL || pListNode == NULL)
    {
        return;
    }

    if (pListNode->pData == NULL)
    {
        return;
    }

    newStatus = GetStatusListIndex(newStatus);

    if (((Process *)pListNode->pData)->status == newStatus)
    {
        return;
    }

    RemoveNodeFromPriorityProcessQueue(&usingListPtr[((Process *)pListNode->pData)->status], pListNode);
    AddNodeToPriorityProcessQueue(&usingListPtr[newStatus], pListNode);
    ((Process *)pListNode->pData)->status = newStatus;
}
