#pragma once
#ifndef PROCESSES_H
#define PROCESSES_H

#ifndef SCHEDULER_CONSTANTS_H
#include "SchedulerConstants.h"
#endif

/*_______________________Structures _______________________*/
typedef struct _process
{
	struct _process *nextProcessPtr[4];	 /* Pointer to a Linked Lists of processes by states*/
	struct _process *nextSiblingProcess; /* Next process in the sibling list */

	struct _process *pParent;	/* Parent process */
	struct _process *pChildren; /* Children processes */

	char name[MAXNAME];				 /* Process name */
	char startArgs[MAXARG];			 /* Process arguments */
	void *context;					 /* Process's current context pointer*/
	short pid;						 /* Process id (pid) */
	int priority;					 /* Process priority */
	int (*entryPoint)(void *);		 /* The entry point pointer that is called from launch */
	char *stack;					 /* Process stack pointer */
	unsigned int stacksize;			 /* Process stack size */
	int signal;						 /* Signal to the process */
	int status;						 /* READY, QUIT, BLOCKED, etc. */
	int exitCode;					 /* Exit code of the process */
	int tableIndex;					 /* Index into the process table */
	unsigned int timeSlice;			 /* Time slice for the process */
	unsigned long startTime;		 /* Time the process was started */
	unsigned long endTime;			 /* Time the process ended */
	unsigned long long waitTime;	 /* Time the process has been waiting */
	unsigned long long elapsedTime;	 /* Time the process has been running */
	unsigned long long blockTime;	 /* Time the process has been blocked */
	unsigned long long demotionTime; /* Time the process has been punished */
	unsigned short demotionCount;	 /* Number of times the process has been punished */
} Process;

/*_______________________Function Prototypes_______________________*/
// PROCESS TABLE FUNCTIONS - Storage for all processes Array of Processes
void initializeProcessTable(Process *usingTablePtr);
void initializeProcessToNull(Process *usingProcessPtr);
int getEmptyControlBlockIndex(Process *fromProcessTablePtr);
void addChildProcess(Process *childPtr, Process *toParentPtr);
void removeChildProcess(Process *childPtr, Process *fromParentPtr);
void removeFromProcessTable(Process *usingProcessPtr, Process *usingTablePtr,
							Process *runningProcess, Process *processList);

void insertIntoProcessTable(Process *newProcessPtr,
							Process *usingTablePtr,
							Process **runningProcess,
							int atTableIndex);

void configureProcessForTable(Process *newProcessPtr, int pid, int tblIndex,
							  char *name, int (*entryPoint)(void *),
							  void *arg, int stacksize, int priority);

// PROCESS LIST FUNCTIONS Array of Linked Lists of Processes indexed by status ie READY, RUNNING, BLOCKED, QUIT

void initializeProcessList(Process **usingProcessListPtr);

void insertIntoProcessList(Process *newProcessNodePtr,
						   Process *usingStatusListPtr,
						   int withStatus,
						   int withPriority);

void removeFromProcessList(Process *processNodePtr, Process **usingProcessListHeadPtr);

#endif
