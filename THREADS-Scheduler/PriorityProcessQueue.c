#include "PriorityProcessQueue.h"

// __________________________ Constants __________________________

const char *STATUS_STRINGS[NUM_PROCESS_STATES] = {
    "READY",
    "RUNNING",
    "WAIT BLOCK",
    "JOIN BLOCK",
    "IO BLOCK",
    "QUIT",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "USER_DEFINED"};
// __________________________ Function Definitions __________________________

/**
 * @brief Get the index of the status in the status list
 *
 * @param status The status to get the index for
 *
 * @return The index of the status list
 */
int GetStatusListIndex(int status)
{
    int safeStatus = status;

    if (safeStatus < 0 || safeStatus > NUM_PROCESS_STATES)
    {
        safeStatus = STATUS_USER_DEFINED;
    }

    return safeStatus;
}

/**
 * @brief Find a static Linked List Node using the process id
 *
 * @param withPid The process id to search for
 * @param pNodeBucket The node bucket, array of nodes, to search
 *
 * @return The linked list node or NULL if not found
 */
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

/**
 * @brief Initialize the static storage node array
 *
 * @param pNodeBucket The array of nodes to initialize
 * @param numNodes The number of nodes to initialize
 */
void InitializeDoublyLinkedNodeStorage(DoublyLinkedNode *pNodeBucket, int numNodes)
{
    int i = 0;

    for (i = 0; i < numNodes; i++)
    {
        InitializeDoublyLinkedNode(0, &pNodeBucket[i], NULL);
    }
}

/**
 * @brief Initializes the priority process queue array
 *
 * @param usingArrayPtr The array of priority process queues
 * @param numStates The number of states to initialize
 */
void InitializePriorityProcessQueueArray(DoublyLinkedList *usingArrayPtr, int numStates)
{
    int i = 0;

    for (i = 0; i < numStates; i++)
    {
        InitializeDoublyLinkedList(0, DOUBLY_LINKED_NODE_OFFSET, &usingArrayPtr[i], orderFunction);
    }
}

/**
 * @brief Add a node to the priority process queue
 *
 * @param usingQueuePtr The queue to add the node to
 * @param pListNode The node to add
 */
void AddNodeToPriorityProcessQueue(DoublyLinkedList *usingQueuePtr, DoublyLinkedNode *pListNode)
{
    if (usingQueuePtr == NULL || pListNode == NULL)
    {
        return;
    }

    InsertNode(pListNode, usingQueuePtr);
}

/**
 * @brief Remove a node from the priority process queue
 *
 * @param usingListPtr The list to remove the node from
 * @param pListNode The node to remove
 */
void RemoveNodeFromPriorityProcessQueue(DoublyLinkedList *usingListPtr, DoublyLinkedNode *pListNode)
{
    if (usingListPtr == NULL || pListNode == NULL)
    {
        return;
    }

    RemoveNode(pListNode, usingListPtr);
}

/**
 * @brief Change the status of a process
 *
 * @param usingListPtr The list to change the status in
 * @param pListNode The node to change the status of
 * @param newStatus The new status to set
 */
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

    MoveDoublyLinkedNode(&usingListPtr[GetStatusListIndex(((Process *)pListNode->pData)->status)],
                         &usingListPtr[safeIndex], pListNode);

    ((Process *)pListNode->pData)->status = newStatus;
}

/**
 * @brief Move a DoublyLinkedNode from one list to another
 *
 * @param pFromList Pointer to the list to move the node from
 * @param pToList Pointer to the list to move the node to
 * @param pNode Pointer to the node to move
 */
void MoveDoublyLinkedNode(DoublyLinkedList *pFromList, DoublyLinkedList *pToList,
                          DoublyLinkedNode *pNode)
{
    // remove the node from the from list
    RemoveNode((void *)pNode, pFromList);
    // insert the node into the to list
    InsertNode((void *)pNode, pToList);
}
