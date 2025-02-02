// #include "Processes.h"
// #include "DoublyLinkedList.h"
#include "PriorityProcessQueue.h"

// __________________________ Function Definitions __________________________

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

    if (newStatus < 0 || newStatus > NUM_PROCESS_STATES)
    {
        return;
    }

    if (((Process *)pListNode->pData)->status == newStatus)
    {
        return;
    }

    RemoveNodeFromPriorityProcessQueue(&usingListPtr[((Process *)pListNode->pData)->status], pListNode);
    AddNodeToPriorityProcessQueue(&usingListPtr[newStatus], pListNode);
    ((Process *)pListNode->pData)->status = newStatus;
}
