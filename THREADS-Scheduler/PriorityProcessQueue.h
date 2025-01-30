#pragma once
#ifndef PRIORITY_PROCESS_QUEUE
#define PRIORITY_PROCESS_QUEUE
#include "Processes.h"
#include "DoublyLinkedList.h"

/*_______________________Function  Definitions_______________________*/

/**
 * @brief Initializes an array of priority process queues.
 *
 * A priority process queue is a doubly linked list of processes that are sorted by priority.
 */
void InitializePriorityProcessQueueArray(DoublyLinkedList* usingArrayPtr, int numStates);
void AddNodeToPriorityProcessQueue(DoublyLinkedList* usingQueuePtr, DoublyLinkedNode* pListNode);
void RemoveNodeFromPriorityProcessQueue(DoublyLinkedList* usingListPtr, DoublyLinkedNode* pListNode);
void ChangeProcessStatus(DoublyLinkedList* usingListPtr, DoublyLinkedNode* pListNode, int newStatus);
#endif
