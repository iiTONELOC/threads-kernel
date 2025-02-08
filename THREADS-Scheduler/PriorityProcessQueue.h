#pragma once
#ifndef PRIORITY_PROCESS_QUEUE
#define PRIORITY_PROCESS_QUEUE
#include "Constants.h"
#include "Processes.h"
#include "DoublyLinkedList.h"

// __________________________ Constants __________________________

const char *STATUS_STRINGS[NUM_PROCESS_STATES];

/*_______________________Function  Prototypes_______________________*/

/**
 * @brief Get the index of the status in the status list
 *
 * @param status The status to get the index for
 *
 * @return The index of the status list
 */
int GetStatusListIndex(int status);

/**
 * @brief Initializes the priority process queue array
 *
 * @param usingArrayPtr The array of priority process queues
 * @param numStates The number of states to initialize
 */
void InitializePriorityProcessQueueArray(DoublyLinkedList *usingArrayPtr, int numStates);

/**
 * @brief Add a node to the priority process queue
 *
 * @param usingQueuePtr The queue to add the node to
 * @param pListNode The node to add
 */
void AddNodeToPriorityProcessQueue(DoublyLinkedList *usingQueuePtr, DoublyLinkedNode *pListNode);

/**
 * @brief Remove a node from the priority process queue
 *
 * @param usingListPtr The list to remove the node from
 * @param pListNode The node to remove
 */
void RemoveNodeFromPriorityProcessQueue(DoublyLinkedList *usingListPtr, DoublyLinkedNode *pListNode);

/**
 * @brief Change the status of a process
 *
 * @param usingListPtr The list to change the status in
 * @param pListNode The node to change the status of
 * @param newStatus The new status to set
 */
void ChangeProcessStatus(DoublyLinkedList *usingListPtr, DoublyLinkedNode *pListNode, int newStatus);

/**
 * @brief Find a static Linked List Node using the process id
 *
 * @param withPid The process id to search for
 * @param pNodeBucket The node bucket, array of nodes, to search
 *
 * @return The linked list node or NULL if not found
 */
DoublyLinkedNode *FindStaticStorageNode(int withPid, DoublyLinkedNode *pNodeBucket);

#endif
