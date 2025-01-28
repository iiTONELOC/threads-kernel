#pragma once
#ifndef PROCESSES_H
#define PROCESSES_H

#ifndef IMPORT_STDLIB
#define IMPORT_STDLIB
#include <stdlib.h>
#endif

#ifndef SCHEDULER_CONSTANTS_H
#include "Constants.h"
#endif

#ifndef LINKEDLIST_H
#include "LinkedList.h"
#endif

/*_______________________Structures _______________________*/
typedef struct _process
{

	LinkedList pChildren;
	LinkedList pDeadChildren;
	LinkedList pJoiningProcesses;

	LinkedListNode *pParent;
	LinkedListNode *nextReadyProcess;

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

int OrderFunction(void *pNode1, void *pNode2);
void InitializeProcessToDefault(Process *usingProcessPtr);
int GetEmptyControlBlockIndex(Process *fromProcessTablePtr);
void InitializeProcessTable(Process *usingTablePtr, size_t size);
LinkedListNode *FindProcessNodeByPid(int pid, LinkedListNode *pNodeBucket);

#endif
