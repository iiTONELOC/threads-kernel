#include "DoublyLinkedList.h"

// __________________________ Function Definitions __________________________

/**
 * @brief Destroys a doubly linked list.
 *
 * This function destroys a doubly linked list.
 *
 * @param pList Pointer to the DoublyLinkedList structure.
 */
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

/**
 * @brief Displays the contents of a doubly linked list.
 *
 * This function displays the contents of a doubly linked list.
 *
 * @param pList Pointer to the DoublyLinkedList structure.
 */
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

/**
 * @brief Creates a new doubly linked list node.
 *
 * This function creates a new doubly linked list node.
 *
 * @param pData Pointer to the data to be attached to the node.
 *
 * @return Pointer to the newly created node or NULL if the memory allocation fails.
 */
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
    pNode->pData = pData; // attach the data to the node
    pNode->dynamic = 1;   // set the dynamic flag to 1

    // Return a pointer to the new node
    return pNode;
}

/**
 * @brief Destroys a doubly linked list node.
 *
 * This function destroys a doubly linked list node.
 *
 * @param pNode Pointer to the node to be destroyed.
 */
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

/**
 * @brief Initializes an array of doubly linked list nodes.
 *
 * This function initializes an array of doubly linked list nodes.
 *
 * @param pNode Pointer to the DoublyLinkedNode structure.
 * @param size The size of the array.
 */
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

/**
 * @brief Get the next empty linked node storage index.
 *
 * This function gets the next empty linked node storage index.
 *
 * @param fromNodeBucket The linked list node bucket to search.
 *
 * @return The index into the linked list node bucket or -1 if the bucket is full.
 */
void InitializeDoublyLinkedNodeStorage(DoublyLinkedNode *pNode, int size)
{
    int i = 0;
    for (i = 0; i < size; i++)
    {
        InitializeDoublyLinkedNode(&pNode[i]);
    }
}

/**
 * @brief Inserts a node into a doubly linked list.
 *
 * This function inserts a node into a doubly linked list in the correct position based on the
 * ordering function provided during initialization.
 *
 * @param pList Pointer to the DoublyLinkedList structure.
 * @param pNode Pointer to the node to be inserted.
 */
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

/**
 * @brief Removes a node from a doubly linked list.
 *
 * This function removes a node from a doubly linked list.
 *
 * @param pList Pointer to the DoublyLinkedList structure.
 * @param pNode Pointer to the node to be removed.
 */
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

    if (current == pList->pHead) // if the node is the head of the list
    {
        // set the head of the list to the next node
        pList->pHead = current->pNext;
        // if the next node is not null, set the previous pointer of the current node to null
        if (current->pNext != NULL)
        {
            current->pNext->pPrev = NULL;
        }
    }
    else if (current == pList->pTail) // if the node is the tail of the list
    {
        // set the tail of the list to the previous node
        pList->pTail = current->pPrev;
        // if the previous node is not null, set the next pointer to null
        if (current->pPrev != NULL)
        {
            current->pPrev->pNext = NULL;
        }
    }
    else // remove somewhere between two ferns
    {
        // set the next pointer of the previous node to the next node
        current->pPrev->pNext = current->pNext;
        // set the previous pointer of the next node to the previous node
        current->pNext->pPrev = current->pPrev;
    }

    // explicit reset the pointers of the current node
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

/**
 * @brief Initializes a doubly linked list node.
 *
 * This function initializes a doubly linked list node.
 *
 * @param pNode Pointer to the DoublyLinkedNode structure.
 */
int GetEmptyNodeArrayStorageIndex(DoublyLinkedNode *fromNodeBucket, int size)
{
    int i;

    for (i = 0; i < size; i++)
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

/**
 * @brief Finds a node in a doubly linked list by a specified value.
 *
 * This function finds a node in a doubly linked list by a specified value.
 *
 * @param pValue Pointer to the value to search for.
 * @param pList Pointer to the DoublyLinkedList structure.
 *
 * @return Pointer to the node containing the value or NULL if the value is not found.
 */
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

/**
 * @brief Creates a new doubly linked list.
 *
 * This function creates a new doubly linked list.
 *
 * @param OrderFunction Pointer to the function used to order the list.
 *
 * @return Pointer to the newly created doubly linked list or NULL if the memory allocation fails.
 */
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

/**
 * @brief Initializes a doubly linked list.
 *
 * This function initializes a doubly linked list.
 *
 * @param pList Pointer to the DoublyLinkedList structure.
 * @param OrderFunction Pointer to the function used to order the list.
 */
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
