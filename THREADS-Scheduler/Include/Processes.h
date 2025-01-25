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
/*_______________________Structures _______________________*/
typedef struct _process
{
	struct _process *nextReadyProcess;
	struct _process *nextSiblingProcess;

	struct _process *pParent;
	struct _process *pChildren;

	char name[MAXNAME];		/* Process name */
	char startArgs[MAXARG]; /* Process arguments */
	void *context;			/* Process's current context */
	short pid;				/* Process id (pid) */
	int priority;
	int (*entryPoint)(void *); /* The entry point that is called from launch */
	char *stack;
	unsigned int stacksize;
	int status; /* READY, QUIT, BLOCKED, etc. */

	unsigned int quantum;
	unsigned int startTime;
	unsigned int elapsedTime;
	unsigned long long cpuTime;

} Process;

/*_______________________Function Prototypes_______________________*/

void InitializeProcessToNull(Process *usingProcessPtr);
int GetEmptyControlBlockIndex(Process *fromProcessTablePtr);
void InitializeProcessTable(Process *usingTablePtr, size_t size);

#endif
