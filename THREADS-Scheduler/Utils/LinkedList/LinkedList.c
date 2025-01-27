/**
 * @file LinkedList.c
 * @see LinkedList.h
 * @author Anthony Tropeano
 * @date  1/23/2025
 *
 * @brief This file contains the implementation for a generic doubly linked list.
 * @note Adapted from Professor Duren's skeleton and Anthony Tropeano's doubly
 *       linked list implementation from the CYBV470 - Final Exercise 3
 *       originally submitted on 12/8/2024.
 */

#ifndef LINKEDLIST_H
#include "LinkedList.h"
#endif

// __________________________ Function Definitions __________________________

void InitializeList(LinkedList *pList, /*int offset,*/ int (*OrderFunction)(void *pNode1, void *pNode2))
{
    if (pList == NULL)
    {
        return;
    }

    pList->count = 0;
    // pList->offset = offset;
    pList->pHead = NULL;
    pList->pTail = NULL;
    pList->OrderFunction = OrderFunction;
}

void InitializeNode(LinkedListNode *pNode)
{
    if (pNode == NULL)
    {
        return;
    }

    pNode->pNext = NULL;
    pNode->pPrev = NULL;
    pNode->pData = NULL;
}

void InsertNode(LinkedList *pList, void *pNode)
{

    // If the list is empty, set the head and tail to the new node
    if (pList->count == 0)
    {
        pList->pHead = pNode;
        pList->pTail = pNode;
    }
    // Otherwise, insert the node in a sorted order
    else
    {
        // Find the correct position to insert the new node
        // it is possible that the order function is null

        // if the order function is null, insert at the end of the list
        if (pList->OrderFunction == NULL)
        {
            // Set the previous pointer of the new node to the current tail
            ((LinkedListNode *)pNode)->pPrev = pList->pTail;

            // Set the next pointer of the current tail to the new node
            ((LinkedListNode *)pList->pTail)->pNext = pNode;

            // Set the tail of the list to the new node
            pList->pTail = pNode;
        }
        // If the new node should be the new head, update the head
        else if (pList->OrderFunction(pNode, pList->pHead) < 0)
        {
            // Set the next pointer of the new node to the current head
            ((LinkedListNode *)pNode)->pNext = pList->pHead;

            // Set the previous pointer of the current head to the new node
            ((LinkedListNode *)pList->pHead)->pPrev = pNode;

            // Set the head of the list to the new node
            pList->pHead = pNode;
        }
        // If the new node should be the new tail, update the tail
        else if (pList->OrderFunction(pNode, pList->pTail) > 0)
        {
            ((LinkedListNode *)pNode)->pPrev = pList->pTail;
            ((LinkedListNode *)pList->pTail)->pNext = pNode;
            pList->pTail = pNode;
        }
        // Otherwise, find the correct position to insert the new node
        else
        {
            LinkedListNode *current = pList->pHead;
            while (current != NULL)
            {
                if (pList->OrderFunction(pNode, current) < 0)
                {
                    ((LinkedListNode *)pNode)->pNext = current;
                    ((LinkedListNode *)pNode)->pPrev = current->pPrev;
                    current->pPrev = pNode;
                    ((LinkedListNode *)(((LinkedListNode *)pNode)->pPrev))->pNext = pNode;
                    break; // exit the while loop condition met
                }

                current = current->pNext; // move to the next node
            }
        }
    }
    pList->count++;
    return;
}

void RemoveNode(LinkedList *pList, void *pNode)
{
    // If the list is empty, return
    if (pList->count == 0)
    {
        return;
    }

    // If the node is the head of the list
    if (pNode == pList->pHead)
    {
        // Set the head of the list to the next node
        pList->pHead = ((LinkedListNode *)pNode)->pNext;

        // If the next node is not NULL, set its previous pointer to NULL
        if (((LinkedListNode *)pNode)->pNext != NULL)
        {
            ((LinkedListNode *)(((LinkedListNode *)pNode)->pNext))->pPrev = NULL;
        }
    }
    // If the node is the tail of the list
    else if (pNode == pList->pTail)
    {
        // Set the tail of the list to the previous node
        pList->pTail = ((LinkedListNode *)pNode)->pPrev;

        // If the previous node is not NULL, set its next pointer to NULL
        if (((LinkedListNode *)pNode)->pPrev != NULL)
        {
            ((LinkedListNode *)(((LinkedListNode *)pNode)->pPrev))->pNext = NULL;
        }
    }
    // Otherwise, remove the node from the list
    else
    {
        // Set the next pointer of the previous node to the next node
        ((LinkedListNode *)(((LinkedListNode *)pNode)->pPrev))->pNext = ((LinkedListNode *)pNode)->pNext;

        // Set the previous pointer of the next node to the previous node
        ((LinkedListNode *)(((LinkedListNode *)pNode)->pNext))->pPrev = ((LinkedListNode *)pNode)->pPrev;
    }

    pList->count--;

    if (pList->count == 0)
    {
        pList->pHead = NULL;
        pList->pTail = NULL;
    }
}

void PrintList(LinkedList *pList)
{
    LinkedListNode *current = pList->pHead;

    while (current != NULL)
    {
        printf("Node: %p\n  Next: %p\n  Prev: %p\n  Data: %p\n", current, current->pNext, current->pPrev, current->pData);
        current = current->pNext;
    }

    printf("\n");
}
