#pragma once

#ifndef LinkedProcessList_H
#define LinkedProcessList_H
#include "../Processes/Process.h"

#ifndef NULL
#define NULL (void *)0
#endif

// ______________________ Constants ______________________

#define MAX_LIST_TYPES 5
#define LIST_TYPE_TO_PROC_MASTER_OFFSET 2

// ______________________ Enumerations ______________________
enum ListType
{
	UNINITIALIZED_LIST,				// 0
	READY_PROCESSES_LIST,			// 1
	PROCESS_CHILDREN_LIST,			// 2
	PROCESS_ZOMBIE_CHILDREN_LIST,	// 3
	PROCESS_EXITING_CHILDREN_LIST,	// 4
	PROCESS_JOINING_PROCESSES_LIST, // 5
};

enum ListType LIST_TYPE;

// ______________________ Structures ______________________

typedef struct LinkedProcessList
{
	int count;
	Process *pHead;
	Process *pTail;

	enum ListType listType;
} LinkedProcessList;

// ______________________ Function Prototypes ______________________

int PLI(enum ListType listType);
Process *PopProcessFromList(LinkedProcessList *pList);
void AddProcessToList(Process *pProcess, LinkedProcessList *pList);
void PushProcessToList(LinkedProcessList *pList, Process *pProcess);
Process **GetNextPtrForList(enum ListType listType, Process *pProcess);
void RemoveProcessFromList(LinkedProcessList *pList, Process *pProcess);
void InitializeProcessList(LinkedProcessList *pList, enum ListType listType);

#endif
