#include "Process.h"
#include "../Utils/Utils.h"

void CreateNewProcess(NewProcessArgs *pProps)
{
    pProps->pNewProcess->signal = 0;                                 // set the signal to an initial value
    pProps->pNewProcess->cpuTime = 0;                                // set the cpu time to an initial value
    pProps->pNewProcess->exitCode = 0;                               // set the exit code to an initial value
    pProps->pNewProcess->startTime = 0;                              // set the start time to an initial value
    pProps->pNewProcess->context = NULL;                             // set the context to NULL
    pProps->pNewProcess->elapsedTime = 0;                            // set the elapsed time to an initial value
    pProps->pNewProcess->joinStatus = -99;                           // explicitly reset the join
    pProps->pNewProcess->pid = pProps->pid;                          // set process id
    pProps->pNewProcess->status = STATUS_READY;                      // set the status to ready
    pProps->pNewProcess->quantum = MAX_PROC_QUANTUM;                 // set the time slice
    pProps->pNewProcess->priority = pProps->priority;                // set process priority
    pProps->pNewProcess->stacksize = pProps->stacksize;              // set process stack size
    pProps->pNewProcess->entryPoint = pProps->entryPoint;            // set process entry point
    pProps->pNewProcess->processTableIndex = pProps->procSlot;       // set the table index
    CopyString(pProps->name, pProps->pNewProcess->name, MAXNAME);    // copy the process name
    CopyString(pProps->arg, pProps->pNewProcess->startArgs, MAXARG); // copy the process arguments
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
