#pragma once
#ifndef PRIORITY_PROCESS_QUEUE
#define PRIORITY_PROCESS_QUEUE
#include "Constants.h"
#include "Processes.h"
#include "DoubleSeaLib.h"
// __________________________ Constants __________________________

const char *STATUS_STRINGS[NUM_PROCESS_STATES];

/*_______________________Function  Prototypes_______________________*/

int GetStatusListIndex(int status);
DSL_Node *FindStaticStorageNode(int withPid, DSL_Node *pNodeBucket);
void InitializeDoublyLinkedNodeStorage(DSL_Node *pNodeBucket, int numNodes);
void InitializePriorityProcessQueueArray(DSL_List *usingArrayPtr, int numStates);
void AddNodeToPriorityProcessQueue(DSL_List *usingQueuePtr, DSL_Node *pListNode);
void RemoveNodeFromPriorityProcessQueue(DSL_List *usingListPtr, DSL_Node *pListNode);
void ChangeProcessStatus(DSL_List *usingListPtr, DSL_Node *pListNode, int newStatus);
void MoveDoublyLinkedNode(DSL_List *pFromList, DSL_List *pToList, DSL_Node *pNode);

#endif
