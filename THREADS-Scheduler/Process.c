#ifndef PROCESSES_H
#include "Processes.h"
#endif
/*_______________________Function  Definitions_______________________*/

/**
 * @brief Initializes a process to NULL
 *
 * @param usingProcessPtr The node to initialize
 */
void InitializeProcessToNull(Process *usingProcessPtr)
{

    usingProcessPtr->pid = NULL;
    usingProcessPtr->stack = NULL;
    usingProcessPtr->status = NULL;
    usingProcessPtr->quantum = NULL; // not currently used by timeslice
    usingProcessPtr->context = NULL;
    usingProcessPtr->cpuTime = NULL;
    usingProcessPtr->name[0] = NULL;
    usingProcessPtr->pParent = NULL;
    usingProcessPtr->priority = NULL;
    usingProcessPtr->startTime = NULL;
    usingProcessPtr->pChildren = NULL;
    usingProcessPtr->stacksize = NULL;
    usingProcessPtr->entryPoint = NULL;
    usingProcessPtr->elapsedTime = NULL;
    usingProcessPtr->startArgs[0] = NULL;
    usingProcessPtr->nextReadyProcess = NULL;
    usingProcessPtr->processTableIndex = NULL;
}

/**
 * @brief Initializes the processes Table
 *
 * Takes an array of Processes and initializes all values to NULL
 *
 * @param usingTablePtr A pointer to the process table to initialize
 * @param size The size of the process table
 */
void InitializeProcessTable(Process *usingTablePtr, size_t size)
{
    size_t i = 0;
    // loop over the array of nodes and initialize each node to
    // NULL values
    for (i = 0; i < size; i++)
    {
        InitializeProcessToNull(&usingTablePtr[i]);
    }
}

/**
 * @brief Retrieve the next empty process slot from the proccess table
 *
 * @param fromProcessTablePtr Pointer to the process table
 *
 * @return The index into the process table or -1 if the table is full
 */
int GetEmptyControlBlockIndex(Process *fromProcessTablePtr)
{
    int i;

    for (i = 0; i < MAXPROC; i++)
    {
        if (fromProcessTablePtr[i].pid == NULL && fromProcessTablePtr[i].context == NULL)
            return i;
    }

    return -1;
}
