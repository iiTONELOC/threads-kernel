#pragma once
#ifndef _SYSTEMCALLS_H
#define _SYSTEMCALLS_H
#define MAX_START_ARG_LEN 512
#include <DoubleSeaLib.h>
#include "SystemCalls.h"

/* -------------------------------- Typedefs and Structs ------------------------------- */

enum SEMAPHORE_STATUS
{
    SEM_FREE = 0,
    SEM_IN_USE = 1,
    SEM_WAITING = 2,
    SEM_INVALID = -1,
    SEM_FREEING = 3,
    SEM_STATUS_COUNT = 4
};

enum USER_PROC_STATUS // Not valid kernel level values - userland only
{
    USER_PROC_FREE = 0,
    USER_PROC_IN_USE = 1,
    USER_PROC_WAITING = 2,
    USER_PROC_INVALID = -1,
    USER_PROC_STATUS_COUNT = 3
};

/* -------------------------------- Typedefs and Structs ------------------------------- */

typedef struct sem_data
{
    int semId;
    int count;
    int status;
    int mutexId; // - mailbox id for the mutex
    int tableIndex;
    int errorOnFree;
    int privateMboxId; // - mailbox id for the semaphore
    DSL_List waitingProcs;
    struct sem_data *pNextSem;
    struct sem_data *pPrevSem;
} SemData;

typedef struct user_proc
{
    int pid;
    int status;
    int priority;
    int tableIndex;
    int privateMboxId;
    int semWaitMboxId;
    DSL_List children;
    int (*startFunc)(char *);
    /*parent*/
    struct user_proc *pParent;
    /*semaphore use*/
    struct user_proc *pNext;
    struct user_proc *pPrev;
    /*parent use*/
    struct user_proc *pNextChild;
    struct user_proc *pPrevChild;
    char startArgs[MAX_START_ARG_LEN];

} UserProcess;

#define MAX_SEMS MAXSEMS                                                 // more readable alias for MAXSEMS
#define SUPPORTED_SYS_CALL_END 20                                        // last supported system call vector index
#define SUPPORTED_SYS_CALL_START 3                                       // first supported system call vector index
#define OFFSETOF_SEM_DATA offsetof(SemData, pNextSem)                    // offset of the next semaphore in the list - NOT SURE IF THIS WILL BE USED
#define OFFSETOF_SEM_DATA_INDEX offsetof(SemData, tableIndex)            // offset of the semaphore index in SemData
#define OFFSETOF_USER_PROC_NEXT_NODE offsetof(UserProcess, pNext)        // offset of the next user process in the list - FOR SEMAPHORE USE
#define OFFSETOF_USER_PROC_CHILD_NODES offsetof(UserProcess, pNextChild) // offset of the next user process in the list - FOR PARENT USE

#endif // _SYSTEMCALLS_H
