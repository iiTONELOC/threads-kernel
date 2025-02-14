#pragma once

#ifndef LinkedProcessList_H
#define LinkedProcessList_H
#include "../Processes/Process.h"

#ifndef NULL
#define NULL (void *)0
#endif

// Process Lists

#define MAX_LIST_TYPES 5
#define LIST_TYPE_TO_PROC_MASTER_OFFSET 2

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

LinkedProcessList GetMasterListForProcess(Process *pProcess);

/**
 * @brief Get the index for the list type
 * Returns the adjusted index for the list type and the process's master list.
 * There are 6 list types, but a process only manages 4 of them. Valid list types
 * start with PROCESS_ and are in the range of 2 to 5.
 *
 * @param listType The type of list to get the index for
 * @return int The index for the list type or -1 if the list type is invalid
 *
 * `Example Usage`
 * ```c
 * int result = GetProcessListIndex(PROCESS_CHILDREN_LIST);
 * // result == 0
 *
 * int result = GetProcessListIndex(READY_PROCESSES_LIST);
 * // result == -1
 * ```
 */
int GetProcessListIndex(enum ListType listType);

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
Process *GetNextPtrForList(enum ListType listType, Process *pProcess);

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
