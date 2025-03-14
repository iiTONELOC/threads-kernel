#pragma once
#ifndef SCHEDULER_UTILS_H
#define SCHEDULER_UTILS_H
#include <stdlib.h>
#include "THREADSLib.h"
#include "PriorityProcessQueue.h"

#define PROCESS_TABLE_ROW_FORMAT "%-7d %-8d %-9d %-13s %-8d %-8llu %s\n"
#define PROCESS_TABLE_HEADER_FORMAT "%-7s %-8s %-9s %-13s %-8s %-8s %-8s\n"

/*_______________________Function Prototypes_______________________*/

void TrimRight(char *pString);
void PrintProcessRow(Process *pProcess);
void DestroyDoublyLinkedNode(DSL_Node *pNode);
DSL_Node *CreateDoublyLinkedNode(void *pData);
void CopyString(char *pSource, char *pDestination, size_t size);
void PrintProcessTable(Process *usingTablePtr, int size, int currentNumProcesses);
int ValidateKSpawnParams(char *name, int (*entryPoint)(void *), void *arg, int stacksize,
                         int priority, int debugFlag);
#endif
