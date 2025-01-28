#ifndef PROCESSES_H
#include "Processes.h"
#endif
/*_______________________Function  Definitions_______________________*/

/**
 * @brief Order function for the test data.
 *
 * @param pNode1 The first process to compare.
 * @param pNode2 The second process to compare.
 *
 * @return The difference between the two priorites.
 */
int OrderFunction(void *pNode1, void *pNode2)
{
    Process *process1 = (Process *)((LinkedListNode *)pNode1)->pData;
    Process *process2 = (Process *)((LinkedListNode *)pNode2)->pData;

    // descending order, the linked list test runs ascending order
    // so here we cover both bases as this function is passed to the
    // linked list's initialization function
    return process2->priority - process1->priority;
}

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
    usingProcessPtr->signal = NULL;
    usingProcessPtr->quantum = NULL; // not currently used by timeslice
    usingProcessPtr->context = NULL;
    usingProcessPtr->cpuTime = NULL;
    usingProcessPtr->pParent = NULL;
    usingProcessPtr->exitCode = NULL;
    usingProcessPtr->priority = NULL;
    usingProcessPtr->startTime = NULL;
    usingProcessPtr->stacksize = NULL;
    usingProcessPtr->entryPoint = NULL;
    usingProcessPtr->elapsedTime = NULL;
    usingProcessPtr->nextReadyProcess = NULL;
    usingProcessPtr->processTableIndex = NULL;
    InitializeList(&usingProcessPtr->pDeadChildren, NULL);
    InitializeList(&usingProcessPtr->pJoiningProcesses, NULL);
    InitializeList(&usingProcessPtr->pChildren, OrderFunction);
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

/**
 * @brief Find a Linked List Node using the process id
 *
 * @param pid The process id to search for
 * @param pNodeBucket The linked list node bucket to search
 *
 * @return The linked list node or NULL if not found
 * @note A pointer to the Process is contained within the pData member of the linked list node
 * A Linked List Node rather than a Process is returned so that the priority list queue can be
 * updated accordingly, and this relies on a linked list node rather than the bare process.
 */
LinkedListNode *FindProcessNodeByPid(int pid, LinkedListNode *pNodeBucket)
{
    int i = 0;
    Process *currentProcess = NULL;
    LinkedListNode *currentNode = NULL;

    // loop over the array of nodes and look for a process with the
    // corresponding pid
    for (i = 0; i < MAXPROC; i++)
    {
        currentNode = &pNodeBucket[i];
        // check to see if pData is null

        if (currentNode == NULL)
        {
            continue;
        }

        currentProcess = (Process *)currentNode->pData;

        if (currentProcess == NULL)
        {
            continue;
        }

        if (currentProcess->pid == pid)
        {
            return currentNode;
        }
    }

    return NULL;
}
