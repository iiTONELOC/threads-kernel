#pragma once
#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdlib.h>
#include "Constants.h"
#include "DoubleSeaLib.h"

/*_______________________Structures _______________________*/
#pragma pack(1)
typedef struct _process
{
	struct _process *pParent;
	DSL_List pChildren;
	DSL_List pDeadChildren;
	DSL_List pExitingChildren;
	DSL_List pJoiningProcesses;

	short pid;					/* Process id (pid) */
	int status;					/* READY, QUIT, BLOCKED, etc. */
	int signal;					/* Signal to send to process */
	char *stack;				/* Process stack */
	int exitCode;				/* Process exit code */
	int priority;				/* Process priority */
	void *context;				/* Process's current context */
	int joinStatus;				/*Status from a process this process is trying to join*/
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

int orderFunction(void *pNode1, void *pNode2);
void InitializeProcessToDefault(Process *usingProcessPtr);
void InitializeNewProcess(Process *usingProcessPtr, char *name,
						  int (*entryPoint)(void *), void *arg,
						  int stacksize, int priority, int procSlot,
						  int nextPid);
void CleanUpAfterChild(Process *pRunningProcess,
					   DSL_List *pChildList,
					   DSL_Node *pStaticStorage,
					   DSL_List *pPriorityListQueue,
					   int *pCode, int *pResult);
int GetEmptyControlBlockIndex(Process *fromProcessTablePtr);
void InitializeProcessTable(Process *usingTablePtr, int size);
void CleanUpPCB(Process *pProcessToClean, DSL_Node *pStaticStorageNode);
Process *GetNextReadyProcess(Process *pRunningProcess, DSL_List *pPriorityListQueue);

#endif
