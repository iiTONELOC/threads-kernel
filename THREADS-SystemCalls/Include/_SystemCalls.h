#pragma once
#ifndef _SYSTEMCALLS_H
#define _SYSTEMCALLS_H
#include <DoubleSeaLib.h>
#include "SystemCalls.h"

/* -------------------------------- Typedefs and Structs ------------------------------- */

typedef struct sem_data
{
    int count;
    int status;
    int sem_id;
    int tableIndex;
    DSL_List waitingProcs;
    struct sem_data *pNextSem;
    struct sem_data *pPrevSem;

    /* Add additional members needed. */
} SemData;

typedef struct user_proc
{
    int pid;
    int status;
    int priority;
    int tableIndex;
    int privateMboxId;
    DSL_List children;
    int (*startFunc)(char *);
    struct user_proc *pParent;
    struct user_proc *pNext;
    struct user_proc *pPrev;
    struct user_proc *pNextChild;
    struct user_proc *pPrevChild;

} UserProcess;

#define MAX_SEMS MAXSEMS
#define SUPPORTED_SYS_CALL_END 20
#define SUPPORTED_SYS_CALL_START 3
#define OFFSETOF_SEM_DATA offsetof(SemData, pNextSem)
#define OFFSETOF_SEM_DATA_INDEX offsetof(SemData, tableIndex)
#define OFFSETOF_USER_PROC_NEXT_NODE offsetof(UserProcess, pNext)
#define OFFSETOF_USER_PROC_CHILD_NODES offsetof(UserProcess, pNextChild)

#endif // _SYSTEMCALLS_H
