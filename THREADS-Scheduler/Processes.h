#pragma once
#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdlib.h>
#include "Constants.h"
#include "DoublyLinkedList.h"

/*_______________________Structures _______________________*/
#pragma pack(1)
typedef struct _process
{
	DoublyLinkedList pChildren;
	DoublyLinkedList pDeadChildren;
	DoublyLinkedList pExitingChildren;
	DoublyLinkedList pJoiningProcesses;

	// LinkedListNode *pParent;
	struct _process *pParent;
	// DoublyLinkedNode* nextReadyProcess;

	short pid;					/* Process id (pid) */
	int status;					/* READY, QUIT, BLOCKED, etc. */
	int signal;					/* Signal to send to process */
	char *stack;				/* Process stack */
	int exitCode;				/* Process exit code */
	int priority;				/* Process priority */
	void *context;				/* Process's current context */
	char name[MAXNAME];			/* Process name */
	unsigned int quantum;		/* Time slice */
	int processTableIndex;		/* Index into the process table */
	unsigned int stacksize;		/* Process stack size */
	char startArgs[MAXARG];		/* Process arguments */
	unsigned int startTime;		/* Process start time */
	unsigned int elapsedTime;	/* Process elapsed time */
	int (*entryPoint)(void *);	/* The entry point that is called from launch */
	unsigned long long cpuTime; /* Process CPU time */

} Process;

/*_______________________Function Prototypes_______________________*/

/**
 * @brief Order function for the test data.
 *
 * @param pNode1 The first process to compare.
 * @param pNode2 The second process to compare.
 *
 * @return The difference between the two priorites.
 */
int OrderFunction(void *pNode1, void *pNode2);

/**
 * @brief Initializes a process to Default values
 *
 * @param usingProcessPtr The node to initialize
 *
 * @note  Pointers are set to NULL, unsigned integers are set to 0, and signed
 *        integers are set to -1. The LinkedLists are initialized. *
 */
void InitializeProcessToDefault(Process *usingProcessPtr);

/**
 * @brief Retrieve the next empty process slot from the proccess table
 *
 * @param fromProcessTablePtr Pointer to the process table
 *
 * @return The index into the process table or -1 if the table is full
 */
int GetEmptyControlBlockIndex(Process *fromProcessTablePtr);

/**
 * @brief Initializes the processes Table
 *
 * Takes an array of Processes and initializes all values to NULL
 *
 * @param usingTablePtr A pointer to the process table to initialize
 * @param size The size of the process table
 */
void InitializeProcessTable(Process *usingTablePtr, int size);

/**
 * @brief Find a Linked List Node using the process id
 *
 * @param pid The process id to search for
 * @param pNodeBucket The linked list node bucket to search
 *
 * @return The linked list node or NULL if not found
 * @note
 *  - A pointer to the Process is contained within the pData member of the linked list node.
 *
 *  - A Linked List Node rather than a Process is returned so that the priority list queue can be
 *   updated accordingly
 *
 *  - This relies on a linked list node rather than the bare process.
 */
DoublyLinkedNode *FindProcessNodeByPid(int pid, DoublyLinkedNode *pNodeBucket);

#endif
