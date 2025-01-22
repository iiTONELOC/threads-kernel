#pragma once

#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#ifndef SCHEDULER_CONSTANTS_H
#include "SchedulerConstants.h"
#endif

#ifndef PROCESSES_H
#include "Processes.h"
#endif

// __________________________ Structures __________________________

typedef struct _processList
{
    Process *headReadyProcessesPtr;   // Ready list of processes index 0
    Process *headRunningProcessesPtr; // Running list of processes index 1
    Process *headBlockedProcessesPtr; // Blocked list of processes index 2
    Process *headQuitProcessesPtr;    // Quit list of processes index 3
} ProcessList;

// __________________________ Function Prototypes __________________________

void initializeProcessList(ProcessList *usingProcessListsPtr);
void insertIntoProcessList(Process *newProcessNodePtr,
                           Process **usingProcessListHeadPtr,
                           int withStatus,
                           int withPriority);
void removeFromProcessList(Process *processNodePtr, Process **usingProcessListHeadPtr);

#endif
