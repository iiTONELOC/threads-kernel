#pragma once

#ifndef LISTS_H
#define LISTS_H
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

#endif
