#pragma once

#ifndef LinkedProcessList_H
#define LinkedProcessList_H
#include "../Processes/Process.h"

#ifndef NULL
#define NULL (void *)0
#endif

enum ListType
{
	UNINITIALIZED_LIST,
	READY_PROCESSES_LIST,
	PROCESS_CHILDREN_LIST,
	PROCESS_ZOMBIE_CHILDREN_LIST,
	PROCESS_EXITING_CHILDREN_LIST,
	PROCESS_JOINING_PROCESSES_LIST,
};

enum ListType LIST_TYPE;

typedef struct LinkedProcessList
{
	int count;
	Process *pHead;
	Process *pTail;
	enum ListType listType;
} LinkedProcessList;

/**
 * @brief Initialize the linked list
 *
 * @param pList  Pointer to the linked list to initialize
 * @param listType  The type of list to initialize
 * @return void
 */
void InitializeProcessList(LinkedProcessList *pList, enum ListType listType);

/**
 * @brief Get the next pointer for the list type
 *
 * @param listType  The type of list to get the next pointer for
 * @param pProcess  Pointer to the process to get the next pointer for
 * @return void
 */
Process *GetNextPtrForListType(enum ListType listType, Process *pProcess);

/**
 * @brief Pop a process from the front of the linked list
 *
 * @param pList  Pointer to the linked list to pop the process from
 * @return Process*  Pointer to the process that was popped from the list
 */
Process *PopProcessFromList(LinkedProcessList *pList);

/**
 * @brief Add a process to the end of the linked list
 *
 * @param pProcess  Pointer to the process to add to the list
 * @param pList  Pointer to the linked list to add the process to
 * @return void
 */
void AddProcessToList(Process *pProcess, LinkedProcessList *pList);

/**
 * @brief Remove a process from the linked list
 *
 * @param pList  Pointer to the linked list to remove the process from
 * @param pProcess  Pointer to the process to remove from the list
 * @return void
 */
void RemoveProcessFromList(LinkedProcessList *pList, Process *pProcess);

#endif
