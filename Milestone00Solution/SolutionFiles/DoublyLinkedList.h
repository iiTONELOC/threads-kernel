#pragma once
#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

#include "Lists.h"
#include <stdio.h>
#include <stdlib.h>

// __________________________ Structures __________________________

/**
 * @struct DoublyLinkedList
 * @brief A doubly linked list.
 *
 * ```c
 * typedef struct DoublyLinkedList
 * {
 *   int count;                                     // Number of nodes in the list
 *   unsigned short dynamic;                           // Dynamic allocation ?
 *   DoublyLinkedNode *pHead;                          // Pointer to the head of the list
 *   DoublyLinkedNode *pTail;                          // Pointer to the tail of the list
 *   int (*OrderFunction)(void *pNode1, void *pNode2); // Pointer to an OrderFunction
 * } DoublyLinkedList;
 * ```
 */
#pragma pack(1)
typedef struct DoublyLinkedList
{
    int count;                                        // Number of nodes in the list
    unsigned short dynamic;                           // Dynamic allocation ?
    DoublyLinkedNode *pHead;                          // Pointer to the head of the list
    DoublyLinkedNode *pTail;                          // Pointer to the tail of the list
    int (*OrderFunction)(void *pNode1, void *pNode2); // Pointer to an OrderFunction
} DoublyLinkedList;

// __________________________ Function Prototypes __________________________

/**
 * @brief Initializes a doubly linked list.
 *
 * This function initializes a doubly linked list.
 *
 * @param pList Pointer to the DoublyLinkedList structure.
 * @param OrderFunction Pointer to the function used to order the list.
 */
void InitializeDoublyLinkedList(DoublyLinkedList *pList,
                                int (*OrderFunction)(void *pNode1, void *pNode2));

/**
 * @brief Initializes an array of doubly linked list nodes.
 *
 * This function initializes an array of doubly linked list nodes.
 *
 * @param pNode Pointer to the DoublyLinkedNode structure.
 * @param size The size of the array.
 */
void InitializeDoublyLinkedNodeStorage(DoublyLinkedNode *pNode, int size);

/**
 * @brief Get the next empty linked node storage index.
 *
 * This function gets the next empty linked node storage index.
 *
 * @param fromNodeBucket The linked list node bucket to search.
 *
 * @return The index into the linked list node bucket or -1 if the bucket is full.
 */
int GetEmptyNodeArrayStorageIndex(DoublyLinkedNode *fromNodeBucket);

/**
 * @brief Initializes a doubly linked list node.
 *
 * This function initializes a doubly linked list node.
 *
 * @param pNode Pointer to the DoublyLinkedNode structure.
 */
void InitializeDoublyLinkedNode(DoublyLinkedNode *pNode);

/**
 * @brief Inserts a node into a doubly linked list.
 *
 * This function inserts a node into a doubly linked list in the correct position based on the
 * ordering function provided during initialization.
 *
 * @param pList Pointer to the DoublyLinkedList structure.
 * @param pNode Pointer to the node to be inserted.
 */
void InsertDoublyLinkedNode(DoublyLinkedList *pList, DoublyLinkedNode *pNode);

/**
 * @brief Removes a node from a doubly linked list.
 *
 * This function removes a node from a doubly linked list.
 *
 * @param pList Pointer to the DoublyLinkedList structure.
 * @param pNode Pointer to the node to be removed.
 */
void RemoveDoublyLinkedNode(DoublyLinkedList *pList, DoublyLinkedNode *pNode);

/**
 * @brief Creates a new doubly linked list node.
 *
 * This function creates a new doubly linked list node.
 *
 * @param pData Pointer to the data to be attached to the node.
 *
 * @return Pointer to the newly created node or NULL if the memory allocation fails.
 */
DoublyLinkedNode *CreateDoublyLinkedNode(void *pData);

/**
 * @brief Destroys a doubly linked list node.
 *
 * This function destroys a doubly linked list node.
 *
 * @param pNode Pointer to the node to be destroyed.
 */
void DestroyDoublyLinkedNode(DoublyLinkedNode *pNode);

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
DoublyLinkedNode *FindDoublyLinkedNode(void *pValue, DoublyLinkedList *pList);

/**
 * @brief Find a static Linked List Node using the process id
 *
 * @param withPid The process id to search for
 * @param pNodeBucket The node bucket, array of nodes, to search
 *
 * @return The linked list node or NULL if not found
 */
DoublyLinkedNode *FindStaticStorageNode(int withPid, DoublyLinkedNode *pNodeBucket);

/**
 * @brief Creates a new doubly linked list.
 *
 * This function creates a new doubly linked list.
 *
 * @param OrderFunction Pointer to the function used to order the list.
 *
 * @return Pointer to the newly created doubly linked list or NULL if the memory allocation fails.
 */
DoublyLinkedList *CreateDoublyLinkedList(int (*OrderFunction)(void *pNode1, void *pNode2));

/**
 * @brief Destroys a doubly linked list.
 *
 * This function destroys a doubly linked list.
 *
 * @param pList Pointer to the DoublyLinkedList structure.
 */
void DestroyDoublyLinkedList(DoublyLinkedList *pList);

/**
 * @brief Displays the contents of a doubly linked list.
 *
 * This function displays the contents of a doubly linked list.
 *
 * @param pList Pointer to the DoublyLinkedList structure.
 */
void DisplayDoublyLinkedList(DoublyLinkedList *pList);

#endif
