
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
            pNode->pPrev = pList->pTail; // Set prev pointer to tail
            pNode->pNext = NULL;         // Set next pointer to NULL

            if (pList->pTail != NULL)
            {
                pList->pTail->pNext = pNode; // Update tail's next pointer
            }
            pList->pTail = pNode; // Set new tail of the list
        }
        else if (pList->OrderFunction(pNode, pList->pHead) < 0)
        {
            // The new node has a lower order than the current head
            pNode->pNext = pList->pHead; // Set next pointer to the current head
            pNode->pPrev = NULL;         // Set prev pointer to NULL
            pList->pHead->pPrev = pNode; // Update the current head's previous pointer
            pList->pHead = pNode;        // Set the head of the list to the new node
        }
        else if (pList->OrderFunction(pNode, pList->pTail) > 0)
        {
            // The new node has a higher order than the current tail
            pNode->pPrev = pList->pTail; // Set prev pointer to the current tail
            pNode->pNext = NULL;         // Set next pointer to NULL
            pList->pTail->pNext = pNode; // Update current tail's next pointer
            pList->pTail = pNode;        // Set the tail of the list to the new node
        }
        else
        {
            // Insert somewhere in between two nodes
            while (current != NULL)
            {
                // Is the value of the new node less than the current node?
                if (pList->OrderFunction(pNode, current) < 0)
                {
                    pNode->pNext = current;        // Set next pointer to the current node
                    pNode->pPrev = current->pPrev; // Set prev pointer to the current node's previous

                    // update the current node's previous node's next pointer
                    if (current->pPrev != NULL)
                    {
                        current->pPrev->pNext = pNode; // Update the previous node's next pointer
                    }
                    else
                    {
                        pList->pHead = pNode; // Inserting at the head, update list head
                    }

                    current->pPrev = pNode; // Update the current node's previous pointer
                    break;                  // Exit the loop once inserted
                }
                // Not less than, move to the next node
                current = current->pNext; // Move to the next node
            }

            // If we finished the loop without inserting, insert at the end
            if (current == NULL)
            {
                pNode->pPrev = pList->pTail; // Set prev pointer to current tail
                pNode->pNext = NULL;         // Set next pointer to NULL
                if (pList->pTail != NULL)
                {
                    pList->pTail->pNext = pNode; // Update current tail's next pointer
                }
                pList->pTail = pNode; // Set the tail of the list to the new node
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
    if (pList == NULL || pList->count == 0)
    {
        return;
    }

    // if the head and the tail are the same, remove the node
    if (pList->pHead == pList->pTail)
    {
        pList->pHead = NULL;
        pList->pTail = NULL;
    }

    // if the node is at the end of the list
    else if (pNode == pList->pTail)
    {
        pList->pTail = pNode->pPrev; // set the tail to the previous node
        pNode->pPrev->pNext = NULL;  // set the next pointer of the previous node to NULL
    }

    // if the node is at the beginning of the list

    else if (pNode == pList->pHead)
    {
        pList->pHead = pNode->pNext; // set the head to the next node
        pNode->pNext->pPrev = NULL;  // set the previous pointer of the next node to NULL
    }

    // if the node is in the middle of the list
    else
    {
        while (current != pNode)
        {
            current = current->pNext;
        }
        current->pPrev->pNext = current->pNext; // update the previous node's next pointer

        // if the current.next is not null, update the next node's previous pointer
        if (current->pNext != NULL)
        {
            current->pNext->pPrev = current->pPrev;
        }

        current = NULL;
    }

    pNode->pNext = NULL;
    pNode->pPrev = NULL;

    // Decrement the count of nodes in the list
    pList->count--;

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
