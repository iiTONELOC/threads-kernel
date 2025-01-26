/**
 * @file LinkedList.h
 * @see LinkedList.c
 * @author Anthony Tropeano
 * @date  1/23/2025
 *
 *  @brief This file contains the declarations for a generic doubly linked list.
 */

#ifndef IMPORT_STDIO
#define IMPORT_STDIO
#include <stdio.h>
#endif

#ifndef LINKEDLIST_H

/**
 * @note This header file conditionally imports the stdio library with it.
 * @note The `LINKEDLIST_H` is the control variable for the linked list header file.
 * @note The `IMPORT_STDIO` is the control variable for the stdio library import.
 * @note When needing to import the stdio library elsewhere, the following code should be used:
 *
 * ```c
 *
 * #ifndef IMPORT_STDIO
 * #define IMPORT_STDIO
 * #include <stdio.h>
 * #endif
 * ```
 */
#define LINKEDLIST_H

// __________________________ Structures __________________________

// Kudos to Professor Duren for the Skeleton!
typedef struct _linkedListNode
{
    void *pNext;
    void *pPrev;
    void *pData;
} LinkedListNode;

typedef struct _linkedList
{
    size_t count;
    void *pHead;
    void *pTail;
    int (*OrderFunction)(void *pNode1, void *pNode2);
} LinkedList;

// __________________________ Function Prototypes __________________________

/**
 * @brief Initializes a linked list.
 *
 * This function initializes a linked list with the specified parameters.
 *
 * @param pList Pointer to the LinkedList structure to be initialized.
 * @param OrderFunction Pointer to a function that defines the ordering of nodes.
 *        The function should take two void pointers to nodes and return an integer
 *        indicating their order. It should return a negative value if the first node
 *        comes before the second, zero if they are equal, and a positive value if the
 *        first node. This is similar to the strcmp function for strings.
 */
void InitializeList(LinkedList *pList, int (*OrderFunction)(void *pNode1, void *pNode2));

/**
 * @brief Initializes a node to NULL values.
 *
 * @param pNode Pointer to the LinkedListNode structure to be initialized.
 */
void InitializeNode(LinkedListNode *pNode);

/**
 * @brief Inserts a node into a linked list.
 *
 * This function inserts a node into a linked list in the correct position based on the
 * ordering function provided during initialization.
 *
 * @param pList Pointer to the LinkedList structure.
 * @param pNode Pointer to the node to be inserted.
 */
void InsertNode(LinkedList *pList, void *pNode);

/**
 * @brief Removes a node from a linked list.
 *
 * This function removes a node from a linked list.
 *
 * @param pList Pointer to the LinkedList structure.
 * @param pNode Pointer to the node to be removed.
 */
void RemoveNode(LinkedList *pList, void *pNode);

/**
 * @brief Prints the contents of a linked list.
 *
 * This function prints the contents of a linked list.
 *
 * @param pList Pointer to the LinkedList structure.
 */
void PrintList(LinkedList *pList);

#endif
