#pragma once

/*_______________________Structures _______________________*/
typedef struct _process
{
	struct _process *nextProcessPtr[3];	 /* Linked Lists of process by states*/
	struct _process *nextSiblingProcess; /* Next process in the sibling list */

	struct _process *pParent;	/* Parent process */
	struct _process *pChildren; /* Children processes */

	char name[MAXNAME];		   /* Process name */
	char startArgs[MAXARG];	   /* Process arguments */
	void *context;			   /* Process's current context pointer*/
	short pid;				   /* Process id (pid) */
	int priority;			   /* Process priority */
	int (*entryPoint)(void *); /* The entry point pointer that is called from launch */
	char *stack;			   /* Process stack pointer */
	unsigned int stacksize;	   /* Process stack size */
	int status;				   /* READY, QUIT, BLOCKED, etc. */
	int tableIndex;			   /* Index into the process table */
} Process;

/*_______________________Function Prototypes_______________________*/

void initializeProcessTable(Process *usingTablePtr);
void initializeProcessToNull(Process *usingProcessPtr);
int getEmptyControlBlockIndex(Process *fromProcessTablePtr);
void addChildProcess(Process *usingProcessPtr, Process **withListHeadPtr);
void removeChildProcess(Process *usingProcessPtr, Process **withListHeadPtr);
void removeFromProcessTable(Process *usingProcessPtr, Process *usingTablePtr,
							Process *runningProcess);
void insertIntoProcessTable(Process *newProcessPtr,
							Process *usingTablePtr,
							Process *runningProcess,
							int atTableIndex);