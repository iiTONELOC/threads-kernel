#pragma once
#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>

// __________________________ Structures __________________________
/**
 * @struct DoublyLinkedNode
 * @brief A node in a doubly linked list.
 *
 * This structure represents a node in a doubly linked list, containing pointers
 * to the next and previous nodes, as well as a pointer to the data it holds.
 *
 * ```c
 * typedef struct DoublyLinkedNode
 * {
 *    void *pData;                    // Pointer to the data the node holds
 *    unsigned short dynamic;         // Flag to indicate if the node was dynamically allocated
 *    struct DoublyLinkedNode *pPrev; // Pointer to the previous node in the list
 *    struct DoublyLinkedNode *pNext; // Pointer to the next node in the list
 * } DoublyLinkedNode;
 * ```
 */
#pragma pack(1)
typedef struct DoublyLinkedNode
{
    void *pData;
    unsigned short dynamic;
    struct DoublyLinkedNode *pNext;
    struct DoublyLinkedNode *pPrev;
} DoublyLinkedNode;

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

void DestroyDoublyLinkedList(DoublyLinkedList *pList);
void DisplayDoublyLinkedList(DoublyLinkedList *pList);
DoublyLinkedNode *CreateDoublyLinkedNode(void *pData);
void DestroyDoublyLinkedNode(DoublyLinkedNode *pNode);
void InitializeDoublyLinkedNode(DoublyLinkedNode *pNode);
void InitializeDoublyLinkedNodeStorage(DoublyLinkedNode *pNode, int size);
void InsertDoublyLinkedNode(DoublyLinkedList *pList, DoublyLinkedNode *pNode);
void RemoveDoublyLinkedNode(DoublyLinkedList *pList, DoublyLinkedNode *pNode);
int GetEmptyNodeArrayStorageIndex(DoublyLinkedNode *fromNodeBucket, int size);
DoublyLinkedNode *FindDoublyLinkedNode(void *pValue, DoublyLinkedList *pList);
DoublyLinkedList *CreateDoublyLinkedList(int (*OrderFunction)(void *pNode1, void *pNode2));
void InitializeDoublyLinkedList(DoublyLinkedList *pList, int (*OrderFunction)(void *pNode1, void *pNode2));

#endif
