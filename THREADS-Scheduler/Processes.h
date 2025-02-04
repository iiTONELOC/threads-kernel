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
	DoublyLinkedList pProcessToJoin;

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
 * @brief Initialize a new process
 *
 * @param usingProcessPtr The process to initialize
 * @param name The name of the process
 * @param entryPoint The entry point of the process
 * @param arg The arguments to pass to the process
 * @param stacksize The size of the stack
 * @param priority The priority of the process
 * @param procSlot The slot in the process table
 * @param nextPid The next process id
 */
void InitializeNewProcess(Process *usingProcessPtr, char *name,
						  int (*entryPoint)(void *), void *arg,
						  int stacksize, int priority, int procSlot,
						  int nextPid);

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
 * @brief Get the next ready process from the READY queue
 *
 * @param pRunningProcess Pointer to the currently running process
 * @param pPriorityListQueue Pointer to the priority list queue
 *
 * @return Pointer to the next ready process or NULL if there are none
 */
Process *GetNextReadyProcess(Process *pRunningProcess,
							 DoublyLinkedList *pPriorityListQueue);

/**
 * @brief Clean up after exited children
 *
 * @param pRunningProcess Pointer to the currently running process
 * @param pChildList Pointer to the list of children
 * @param pStaticStorage Pointer to the static storage array
 * @param pPriorityListQueue Pointer to the priority list queue
 * @param pCode Pointer to the exit code
 * @param pResult Pointer to the result
 */
void CleanUpAfterChild(Process *pRunningProcess,
					   DoublyLinkedList *pChildList,
					   DoublyLinkedNode *pStaticStorage,
					   DoublyLinkedList *pPriorityListQueue,
					   int *pCode, int *pResult);
#endif
