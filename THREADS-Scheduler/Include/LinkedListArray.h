/**
 * @file LinkedListArray.h
 * @brief This file contains the declarations for a generic linked list array.
 * @author Anthony Tropeano
 * @date  1/23/2025
 *
 * @note The purpose of the linked list array is to provide a bucket for storage
 *       so that malloc and free operations are not required for each node.
 *
 *      How the array is declared is up to the programmer. The array can be
 *      declared as a static array or dynamically allocated. The array could then
 *      be extended if needed but this linked list array makes no assumptions,
 *      except that the data is stored in a pre-allocated array.
 *
 */

#ifndef LINKEDLIST_H
#include "LinkedList.h"
#endif

#ifndef LINKEDLISTARRAY_H
#define LINKEDLISTARRAY_H

// __________________________ Structures __________________________

typedef struct _linkedListArray
{
    size_t nodeCount;             // The number of nodes in the array
    LinkedList *pLinkedList;      // The linked list structure
    LinkedListNode *pNodeStorage; // The array of nodes
} LinkedListArray;

// __________________________ Function Prototypes __________________________

/**
 * @brief Initializes a linked list array.
 *
 * This function initializes a linked list array with the specified parameters.
 *
 * @param pListArray Pointer to the LinkedListArray structure to be initialized.
 * @param pList Pointer to the LinkedList structure to be used in the array.
 * @param pNodeStorage Pointer to the array of nodes to be used for storage.
 * @param nodeCount The number of nodes in the array.
 * @param OrderFunction Pointer to a function that defines the ordering of nodes.
 */
void InitializeLinkedListArray(LinkedListArray *pListArray, LinkedList *pList,
                               LinkedListNode *pNodeStorage, size_t nodeCount,
                               int (*OrderFunction)(void *pNode1, void *pNode2));

/**
 * @brief Returns the index to the next empty node in the array.
 *
 * This function returns the index of the next empty node in the array.
 *
 * @param pListArray Pointer to the LinkedListArray structure.
 *
 * @return int The index of the next empty node or -1 if the array is full.
 */
int GetNextEmptyIndex(LinkedListArray *pListArray);

/**
 * @brief Inserts a node into the linked list array.
 *
 * This function inserts a node into the linked list array in the correct position based on the
 * ordering function provided during initialization.
 *
 * @param pListArray Pointer to the LinkedListArray structure.
 * @param pData Pointer to the data to be inserted, ie, a pointer to the structure to be stored
 *            in the node.
 *
 * @return int The index of the inserted node or -1 if the node was not inserted.
 */
int InsertDataIntoLinkedListArray(LinkedListArray *pListArray, void *pData);

/**
 * @brief Removes a node from the linked list array.
 *
 * This function removes a node from the linked list array.
 *
 * @param pListArray Pointer to the LinkedListArray structure.
 * @param pNode Pointer to the node to be removed.
 *
 * @return int The index of the removed node or -1 if the node was not found.
 */
int RemoveNodeFromLinkedListArray(LinkedListArray *pListArray, LinkedListNode *pNode);

#endif
