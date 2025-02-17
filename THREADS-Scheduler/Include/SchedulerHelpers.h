#pragma once
#ifndef SCHEDULER_HELPERS_H
#define SCHEDULER_HELPERS_H

#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"

#include "../Utils/Utils.h"
#include "../LinkedProcessList/LinkedProcessList.h"

typedef Process ProcessTable[MAX_PROCESSES];
typedef LinkedProcessList MasterList[MAX_PROCESSES][NUM_UNIQUE_LISTS], ReadyList[NUM_PRIORITIES],
    ChildrenList, ExitingChildrenList, JoinerList, ZombieChildrenList;

typedef void *(disableInterruptsFn)();

#define NUM_MILLI_SEC_IN_MICRO_SEC 1000

extern int nextPid;
extern int processCount;
extern ReadyList readyList;
extern Process *runningProcess;
extern ProcessTable processTable;
extern MasterList linkedListMaster;

void IncrementPid();
void SchedulerInitReadyList();
int SchedulerPidToIndex(int pid);
Process *SchedulerGetNextProcess();
unsigned int SchedulerCalculateTimeSlice();
void SchedulerCleanUpProcess(Process *pProcess);
void SchedulerCreateNewProcess(NewProcessArgs *pProps);
void SchedulerHandleContextSwitch(Process *pNextProcess);
// -- Could arguably be moved to a SchedulerProcessHelpers --

void WakeUpJoiners();
void ChildNotifyParentOfExit();
void PrintProcessRow(Process *pProcess);
void PrintProcessTable(Process *usingTablePtr, int size);
int HandleZombieChildren(Process *pProcess, int *pExitCode);
int HandleExitingChildren(Process *pProcess, int *pExitCode);
void CleanUpChildProcess(Process *pProcess, int *pChildExitCode, int *pResult);

#endif