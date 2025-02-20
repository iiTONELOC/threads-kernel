#pragma once
#ifndef PRIORITY_PROCESS_QUEUE
#define PRIORITY_PROCESS_QUEUE
#include "Constants.h"
#include "Processes.h"
#include "DoublyLinkedList.h"
// __________________________ Constants __________________________

const char *STATUS_STRINGS[NUM_PROCESS_STATES];

/*_______________________Function  Prototypes_______________________*/

int GetStatusListIndex(int status);
DoublyLinkedNode *FindStaticStorageNode(int withPid, DoublyLinkedNode *pNodeBucket);
void InitializeDoublyLinkedNodeStorage(DoublyLinkedNode *pNodeBucket, int numNodes);
void InitializePriorityProcessQueueArray(DoublyLinkedList *usingArrayPtr, int numStates);
void AddNodeToPriorityProcessQueue(DoublyLinkedList *usingQueuePtr, DoublyLinkedNode *pListNode);
void RemoveNodeFromPriorityProcessQueue(DoublyLinkedList *usingListPtr, DoublyLinkedNode *pListNode);
void ChangeProcessStatus(DoublyLinkedList *usingListPtr, DoublyLinkedNode *pListNode, int newStatus);
void MoveDoublyLinkedNode(DoublyLinkedList *pFromList, DoublyLinkedList *pToList, DoublyLinkedNode *pNode);

#endif
