#pragma once
#ifndef PROCESS_H
#define PROCESS_H
#include "../Utils/Utils.h"

#ifndef NULL
#define NULL (void *)0
#endif

// _______________________________ Constants _______________________________

// Swappable Constants
#ifndef MAXNAME
#define MAXNAME 256
#endif

#ifndef MAXARG
#define MAXARG 256
#endif

#ifndef MAXPROC
#define MAXPROC 50
#endif

// Process Status
#define STATUS_LAST_RESERVED = 10;

enum ProcessStatus
{
    STATUS_UNINITIALIZED,   // 0
    STATUS_READY,           // 1
    STATUS_RUNNING,         // 2
    STATUS_BLOCKED_ON_IO,   // 3
    STATUS_BLOCKED_ON_JOIN, // 4
    STATUS_BLOCKED_ON_WAIT, // 5
    STATUS_QUIT,            // 6
};

// Process Priorities
enum ProcessPriority
{
    PRIORITY_0, // 0
    PRIORITY_1, // 1
    PRIORITY_2, // 2
    PRIORITY_3, // 3
    PRIORITY_4, // 4
    PRIORITY_5  // 5
};

#define NUM_PRIORITIES 6

// Process Lists
#define NUM_UNIQUE_LISTS 4

// Process Quantum
#ifndef MAX_PROC_QUANTUM
#define MAX_PROC_QUANTUM 80 * 1000 // 80 ms in microseconds
#endif

// _______________________________ Structures _______________________________

typedef struct Process
{
    // -- Managed by the Process in the Scheduler --
    struct Process *pParent;           /* Pointer to the parent process */
    struct Process *pNextChild;        /* Pointer to the next child process */
    struct Process *pNextJoiner;       /* Pointer to the next joiner process */
    struct Process *pNextZombieChild;  /* Pointer to the next zombie child process */
    struct Process *pNextExitingChild; /* Pointer to the next exiting child process */
    // -- Managed by the Scheduler in the Dispatcher --
    struct Process *pNextReadyProcess; /* Pointer to the next ready process */

    short pid;                  /* Process id (pid) */
    int status;                 /* READY, QUIT, BLOCKED, etc. */
    int signal;                 /* Signal to send to process */
    char *stack;                /* Process stack */
    int exitCode;               /* Process exit code */
    int priority;               /* Process priority */
    void *context;              /* Process's current context */
    int joinStatus;             /*Status from a process this process is trying to join*/
    char name[MAXNAME];         /* Process name */
    unsigned int quantum;       /* Time slice */
    int processTableIndex;      /* Index into the process table */
    unsigned int stacksize;     /* Process stack size */
    char startArgs[MAXARG];     /* Process arguments */
    unsigned int startTime;     /* Process start time */
    unsigned int elapsedTime;   /* Process elapsed time */
    int (*entryPoint)(void *);  /* The entry point that is called from launch */
    unsigned long long cpuTime; /* Process CPU time */
} Process;

/**
 * @brief Arguments for creating a new process
 *
 * @param pid The process id
 * @param arg The process arguments
 * @param name The process name
 * @param priority The process priority
 * @param procSlot The process slot
 * @param stacksize The process stack size
 * @param pNewProcess The new process
 * @param entryPoint The process entry point
 */
typedef struct NewProcessArgs
{
    int pid;
    void *arg;
    char *name;
    int priority;
    int procSlot;
    int stacksize;
    Process *pNewProcess;
    int (*entryPoint)(void *);
} NewProcessArgs;

// _______________________________ Function Prototypes _______________________________

void CreateNewProcess(NewProcessArgs *pProps);

int GetEmptyControlBlockIndex(Process *fromProcessTablePtr);

#endif
