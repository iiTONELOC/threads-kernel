
#include "DoublyLinkedList.h"
#include "Constants.h"

// __________________________ Function Definitions __________________________

void InitializeDoublyLinkedList(DoublyLinkedList *pList,
                                int (*OrderFunction)(void *pNode1, void *pNode2))
{
    if (pList == NULL)
    {
        return;
    }

    pList->count = 0;
    pList->pHead = NULL;
    pList->pTail = NULL;
    pList->dynamic = 0;
    pList->OrderFunction = OrderFunction == NULL ? NULL : OrderFunction;
}

void InitializeDoublyLinkedNode(DoublyLinkedNode *pNode)
{
    if (pNode == NULL)
    {
        return;
    }
    pNode->dynamic = 0;
    pNode->pNext = NULL;
    pNode->pPrev = NULL;
    pNode->pData = NULL;
}

void InitializeDoublyLinkedNodeStorage(DoublyLinkedNode *pNode, size_t size)
{
    size_t i = 0;
    for (i = 0; i < size; i++)
    {
        InitializeDoublyLinkedNode(&pNode[i]);
    }
}

int GetEmptyNodeArrayStorageIndex(DoublyLinkedNode *fromNodeBucket)
{
    int i;

    for (i = 0; i < MAXPROC; i++)
    { // if a pid hasn't been assigned and the context is NULL
        // we have a slot we can use - regardless of whatever else is in the slot
        if (fromNodeBucket[i].pData == NULL)
        {
            // reset the next and previous pointers
            fromNodeBucket[i].pNext = fromNodeBucket[i].pPrev = NULL;
            return i;
        }
    }

    return -1;
}

void InsertDoublyLinkedNode(DoublyLinkedList *pList, DoublyLinkedNode *pNode)
{
    // Check for invalid pointers
    if (pList == NULL || pNode == NULL)
    {
        return;
    }

    // If the list is empty, set head and tail to the new node
    if (pList->count == 0)
    {
        pList->pHead = pNode; // Set head of the list
        pList->pTail = pNode; // Set tail of the list
        pNode->pNext = NULL;  // Set next pointer to NULL
        pNode->pPrev = NULL;  // Set prev pointer to NULL
    }
    else // List isn't empty; find where to insert
    {
        DoublyLinkedNode *current = pList->pHead;

        // If there is no order function, insert at the end
        if (pList->OrderFunction == NULL)
        {
            // this new node is the new tail of the list

            // set the next pointer of the current tail to the new node
            pList->pTail->pNext = pNode;
            // set the previous pointer of the new node to the current tail
            pNode->pPrev = pList->pTail;
            // set the tail of the list to the new node
            pList->pTail = pNode;
        }
        else if (pList->OrderFunction(pNode, pList->pHead) < 0)
        {
            // The new node is the new head of the list

            // set the next pointer of the new node to the current head
            pNode->pNext = pList->pHead;
            // set the previous pointer of the current head to the new node
            pList->pHead->pPrev = pNode;
            // set the head of the list to the new node
            pList->pHead = pNode;
        }
        else if (pList->OrderFunction(pNode, pList->pTail) > 0)
        {
            // The new node has a higher order than the current tail

            // set the next pointer of the current tail to the new node
            pList->pTail->pNext = pNode;
            // set the previous pointer of the new node to the current tail
            pNode->pPrev = pList->pTail;
            // set the tail of the list to the new node
            pList->pTail = pNode;
        }
        else
        {
            // The new node is somewhere in between the head and tail
            // we need to insert the new node between the current node and the previous node
            while (current != NULL)
            {
                if (pList->OrderFunction(pNode, current) < 0)
                {
                    break;
                }
                current = current->pNext;
            }

            // if the current node is NULL, the new node is the new tail
            // Shouldn't happen, as we check for this case above, but...
            if (current == NULL)
            {
                // set the previous pointer of the new node to the current tail
                pNode->pPrev = pList->pTail;
                // set the next pointer of the current tail to the new node
                pList->pTail->pNext = pNode;
                // set the tail of the list to the new node
                pList->pTail = pNode;
            }
            else
            {
                // the new node is somewhere in between the head and tail
                // we need to insert the new node between the current node and the previous node
                // set the next pointer of the new node to the current node
                pNode->pNext = current;
                // set the previous pointer of the new node to the previous node
                pNode->pPrev = current->pPrev;
                // set the next pointer of the previous node to the new node
                current->pPrev->pNext = pNode;
                // set the previous pointer of the current node to the new node
                current->pPrev = pNode;
            }
        }
    }
    pList->count++; // Increment the count of nodes in the list
}

void RemoveDoublyLinkedNode(DoublyLinkedList *pList, DoublyLinkedNode *pNode)
{
    /**https://stackoverflow.com/questions/47045583/remove-method-for-a-doubly-linked-list */

    DoublyLinkedNode *current = pList->pHead;
    // if the list is empty, return
    if (pList == NULL || pList->count == 0 || pNode == NULL)
    {
        return;
    }

    // look for the node in the list
    while (current != NULL)
    {
        if (current == pNode)
        {
            break;
        }
        current = current->pNext;
    }

    // if the node is not found, return
    if (current == NULL)
    {
        return;
    }

    // if the node is the head of the list
    if (current == pList->pHead)
    {
        // set the head of the list to the next node
        pList->pHead = current->pNext;
        // if the next node is not null, set the previous pointer of the current node to null
        if (current->pNext != NULL)
        {
            current->pNext->pPrev = NULL;
        }
    }
    else if (current == pList->pTail)
    {
        // set the tail of the list to the previous node
        pList->pTail = current->pPrev;
        // if the previous node is not null, set the next pointer to null
        if (current->pPrev != NULL)
        {
            current->pPrev->pNext = NULL;
        }
    }
    else
    {
        // remove somewhere between two ferns

        // set the next pointer of the previous node to the next node
        current->pPrev->pNext = current->pNext;
        // set the previous pointer of the next node to the previous node
        current->pNext->pPrev = current->pPrev;
    }

    // set the next and previous pointers of the current node to null
    current->pNext = NULL;
    current->pPrev = NULL;

    pList->count--; // decrement the count of nodes in the list

    // if the head is null and the count is 1, move the tail to the head
    if (pList->pHead == NULL && pList->count == 1)
    {
        pList->pHead = pList->pTail;
        pList->pHead->pPrev = NULL;
        pList->pHead->pNext = NULL;
    }

    // If the list is empty now, set both head and tail to NULL
    if (pList->count == 0)
    {
        pList->pHead = NULL;
        pList->pTail = NULL;
    }
}

DoublyLinkedNode *CreateDoublyLinkedNode(void *pData)
{
    // Allocate memory for the new node
    DoublyLinkedNode *pNode = (DoublyLinkedNode *)malloc(sizeof(DoublyLinkedNode));

    // If memory allocation fails, return NULL
    if (pNode == NULL)
    {
        return NULL;
    }

    // Initialize the new node
    InitializeDoublyLinkedNode(pNode);
    pNode->pData = pData;
    pNode->dynamic = 1;

    // Return a pointer to the new node
    return pNode;
}

void DestroyDoublyLinkedNode(DoublyLinkedNode *pNode)
{
    // If the node is NULL, return
    if (pNode == NULL)
    {
        return;
    }

    // If the node is dynamic, free the memory
    if (pNode->dynamic)
    {
        free(pNode);
    }
}

DoublyLinkedNode *FindDoublyLinkedNode(void *pValue, DoublyLinkedList *pList)
{
    // If the list is NULL or empty, return NULL
    if (pList == NULL || pList->count == 0)
    {
        return NULL;
    }

    // Start at the head of the list
    DoublyLinkedNode *current = pList->pHead;

    // Traverse the list until the value is found or the end of the list is reached
    while (current != NULL)
    {
        if (current->pData == pValue)
        {
            return current;
        }
        current = current->pNext;
    }

    // If the value is not found, return NULL
    return NULL;
}

DoublyLinkedList *CreateDoublyLinkedList(int (*OrderFunction)(void *pNode1, void *pNode2))
{
    // Allocate memory for the new list
    DoublyLinkedList *pList = (DoublyLinkedList *)malloc(sizeof(DoublyLinkedList));

    // If memory allocation fails, return NULL
    if (pList == NULL)
    {
        return NULL;
    }

    // Initialize the new list
    InitializeDoublyLinkedList(pList, OrderFunction == NULL ? NULL : OrderFunction);

    // Return a pointer to the new list
    return pList;
}

void DestroyDoublyLinkedList(DoublyLinkedList *pList)
{
    DoublyLinkedNode *current, *next;

    // If the list is NULL, return
    if (pList == NULL)
    {
        return;
    }

    // If the list is dynamic, free the memory
    if (pList->dynamic)
    {
        // loop through the list and free all nodes
        current = pList->pHead;
        while (current != NULL)
        {
            next = current->pNext;
            DestroyDoublyLinkedNode(current);
            current = next;
        }
        free(pList);
    }
}

void DisplayDoublyLinkedList(DoublyLinkedList *pList)
{
    size_t i = 1;
    // If the list is NULL or empty, return
    if (pList == NULL || pList->count == 0)
    {
        return;
    }

    // Start at the head of the list
    DoublyLinkedNode *current = pList->pHead;

    // Traverse the list and display the contents of each node
    while (current != NULL)
    {
        // Display the contents of the current node
        printf("Node %zu: %p\n", i, current->pData);

        // Move to the next node
        current = current->pNext;
        i++;
    }
}
