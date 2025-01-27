/**
 * @file LinkedListArray.c
 * @see LinkedListArray.h
 * @see LinkedList.h
 * @author Anthony Tropeano
 * @date  1/23/2025
 *
 * @brief This file contains the implementation for a generic linked list array.
 *        This allows for a bucket of nodes to be used for storage so that malloc
 *       and free operations are not required for each node, or extremely infrequently.
 */

#ifndef LINKEDLISTARRAY_H
#include "LinkedListArray.h"
#endif

// __________________________ Function Definitions __________________________

void InitializeLinkedListArray(LinkedListArray *pListArray, LinkedList *pList,
                               LinkedListNode *pNodeStorage, size_t nodeCount,
                               int (*OrderFunction)(void *pNode1, void *pNode2))
{
    if (pListArray == NULL)
    {
        return;
    }

    // get the linked list and node storage array
    pListArray->pLinkedList = pList;
    pListArray->nodeCount = nodeCount;
    pListArray->pNodeStorage = pNodeStorage;

    // initialize the linked list and set the order function
    InitializeList(pListArray->pLinkedList, OrderFunction);

    // initialize the node storage array, by setting all pointers to NULL
    for (size_t i = 0; i < nodeCount; i++)
    {
        InitializeNode(&pNodeStorage[i]);
    }
}

int GetNextEmptyIndex(LinkedListArray *pListArray)
{
    // loop over the array of nodes and find the next empty node
    for (size_t i = 0; i < pListArray->nodeCount; i++)
    {
        if (pListArray->pNodeStorage[i].pData == NULL)
        {
            return i;
        }
    }

    // no empty nodes found
    return -1;
}

int InsertDataIntoLinkedListArray(LinkedListArray *pListArray, void *pData)
{
    // get the next empty index in the array
    int nextEmptyIndex = GetNextEmptyIndex(pListArray);

    // if the next empty index is not found, return
    if (nextEmptyIndex == -1)
    {
        return -1;
    }

    // set the data pointer of the node to the node, leave the next and prev pointers as NULL
    pListArray->pNodeStorage[nextEmptyIndex].pData = pData;

    // insert the node into the linked list
    InsertNode(pListArray->pLinkedList, &pListArray->pNodeStorage[nextEmptyIndex]);

    return nextEmptyIndex;
}

int RemoveNodeFromLinkedListArray(LinkedListArray *pListArray, LinkedListNode *pNode)
{
    // loop over the array of nodes and find the node to remove
    for (size_t i = 0; i < pListArray->nodeCount; i++)
    {
        if (&pListArray->pNodeStorage[i] == pNode)
        {
            // remove the node from the linked list
            RemoveNode(pListArray->pLinkedList, pNode);

            // reset the node to default values
            InitializeNode(&pListArray->pNodeStorage[i]);

            return i;
        }
    }

    // node not found
    return -1;
}