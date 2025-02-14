#include "Process.h"
#include "../Utils/Utils.h"

void InitializeNewProcess(NewProcessArgs *pProps)
{
    pProps->pNewProcess->signal = 0;                              // set the signal to an initial value
    pProps->pNewProcess->cpuTime = 0;                             // set the cpu time to an initial value
    pProps->pNewProcess->startTime = 0;                           // set the start time to an initial value
    pProps->pNewProcess->elapsedTime = 0;                         // set the elapsed time to an initial value
    pProps->pNewProcess->joinStatus = -99;                        // explicitly reset the join
    pProps->pNewProcess->pid = pProps->pid;                       // set process id
    pProps->pNewProcess->status = STATUS_READY;                   // set the status to ready
    pProps->pNewProcess->quantum = MAX_PROC_QUANTUM;              // set the time slice
    pProps->pNewProcess->priority = pProps->priority;             // set process priority
    pProps->pNewProcess->stacksize = pProps->stacksize;           // set process stack size
    pProps->pNewProcess->entryPoint = pProps->entryPoint;         // set process entry point
    pProps->pNewProcess->processTableIndex = pProps->procSlot;    // set the table index
    CopyString(pProps->name, pProps->pNewProcess->name, MAXNAME); // Copy process name - shouldn't be NULL
    // Copy process arguments - might be NULL
    if (pProps->arg != NULL)
    {
        CopyString(pProps->arg, pProps->pNewProcess->startArgs, MAXARG);
    }
    else
    {
        pProps->pNewProcess->startArgs[0] = '\0'; // Empty string if no arguments
    }
}

int GetEmptyControlBlockIndex(Process *fromProcessTablePtr)
{
    int i;

    for (i = 0; i < MAXPROC; i++)
    {
        if (fromProcessTablePtr[i].pid == 0)
        {
            return i;
        }
    }

    return -1;
}
