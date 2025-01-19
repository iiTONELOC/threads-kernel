#include "SchedulerConstants.h"
#include "Processes.h"
#pragma once
// __________________________ Structures __________________________

typedef struct _processList
{
    Process *headReadyProcessesPtr;   // Ready list of processes index 0
    Process *headRunningProcessesPtr; // Running list of processes index 1
    Process *headBlockedProcessesPtr; // Blocked list of processes index 2
} ProcessList;

// __________________________ Function Prototypes __________________________

void initializeProcessList(ProcessList *usingProcessListsPtr);
void insertIntoProcessList(Process *newProcessNodePtr,
                           Process **usingProcessListHeadPtr,
                           int withStatus,
                           int withPriority);
void removeFromProcessList(Process *processNodePtr, Process **usingProcessListHeadPtr);
