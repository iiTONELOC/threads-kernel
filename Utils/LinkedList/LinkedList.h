/**
 * @file LinkedList.h
 * @see LinkedList.c
 * @author Anthony Tropeano
 * @date  1/23/2025
 *
 *  @brief This file contains the declarations for a generic doubly linked list.
 */

// Add the standard library import here since it is a dependency, this ensures
// that the LinkedList has access to the standard library even if the developer
// forgets to include it in their main file.
#ifndef IMPORT_STDLIB
#define IMPORT_STDLIB
#include <stdlib.h>
#endif

#ifndef LINKEDLIST_H

/**
 * @note This header file conditionally imports the standard library with it.
 * @note The `LINKEDLIST_H` is the control variable for the linked list header file.
 * @note The `IMPORT_STDLIB` is the control variable for the standard library import.
 * @note When needing to import the standard library elsewhere, the following code should be used:
 *
 * ```c
 *
 * #ifndef IMPORT_STDLIB
 * #define IMPORT_STDLIB
 * #include <stdlib.h>
 * #endif
 * ```
 */
#define LINKEDLIST_H

// __________________________ Structures __________________________
#pragma pack(1)

// Kudos to Professor Duren for the Skeleton!
typedef struct _linkedListNode
{
    void *pNext;
    void *pPrev;
} LinkedListNode;

typedef struct _linkedList
{
    int count;
    int offset;
    void *pHead;
    void *pTail;
    int (*OrderFunction)(void *pNode1, void *pNode2);
} LinkedList;

// __________________________ Functions __________________________

/**
 * @brief Initializes a linked list.
 *
 * This function initializes a linked list with the specified parameters.
 *
 * @param pList Pointer to the LinkedList structure to be initialized.
 * @param offset Offset value used for node positioning within the list.
 * @param OrderFunction Pointer to a function that defines the ordering of nodes.
 *        The function should take two void pointers to nodes and return an integer
 *        indicating their order. It should return a negative value if the first node
 *        should come before the second, zero if they are equal, and a positive value
 *        if the first node should come after the second. This is similar to the strcmp
 *        function for strings.
 */
static void InitializeList(LinkedList *pList, int offset, int (*OrderFunction)(void *pNode1, void *pNode2));

/**
 * @brief Inserts a node into a linked list.
 *
 * This function inserts a node into a linked list in the correct position based on the
 * ordering function provided during initialization.
 *
 * @param pList Pointer to the LinkedList structure.
 * @param pNode Pointer to the node to be inserted.
 */
static void InsertNode(LinkedList *pList, void *pNode);

/**
 * @brief Removes a node from a linked list.
 *
 * This function removes a node from a linked list.
 *
 * @param pList Pointer to the LinkedList structure.
 * @param pNode Pointer to the node to be removed.
 */
static void RemoveNode(LinkedList *pList, void *pNode);

/**
 * @brief Prints the contents of a linked list.
 *
 * This function prints the contents of a linked list.
 *
 * @param pList Pointer to the LinkedList structure.
 */
static void PrintList(LinkedList *pList);

#endif
