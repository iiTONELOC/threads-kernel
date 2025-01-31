#include "Processes.h"
#include "Constants.h"

/*_______________________Function  Definitions_______________________*/

int OrderFunction(void *pNode1, void *pNode2)
{
    Process *process1 = (Process *)((DoublyLinkedNode *)pNode1)->pData;
    Process *process2 = (Process *)((DoublyLinkedNode *)pNode2)->pData;

    if (process1 == NULL || process2 == NULL)
    {
        return 0;
    }

    // descending order, the linked list test runs ascending order
    // so here we cover both bases as this function is passed to the
    // linked list's initialization function
    return process2->priority - process1->priority;
}

void InitializeProcessToDefault(Process *usingProcessPtr)
{
    usingProcessPtr->pid = -1;
    usingProcessPtr->status = -1;
    usingProcessPtr->signal = -1;
    usingProcessPtr->quantum = 0; // not currently used by timeslice
    usingProcessPtr->cpuTime = 0;
    usingProcessPtr->stack = NULL;
    usingProcessPtr->exitCode = -1;
    usingProcessPtr->priority = -1;
    usingProcessPtr->startTime = -1;
    usingProcessPtr->stacksize = -1;
    usingProcessPtr->context = NULL;
    usingProcessPtr->pParent = NULL;
    usingProcessPtr->elapsedTime = -1;
    usingProcessPtr->entryPoint = NULL;
    usingProcessPtr->processTableIndex = -1;
    InitializeDoublyLinkedList(&usingProcessPtr->pChildren, NULL);
    InitializeDoublyLinkedList(&usingProcessPtr->pDeadChildren, NULL);
    InitializeDoublyLinkedList(&usingProcessPtr->pExitingChildren, NULL);
    InitializeDoublyLinkedList(&usingProcessPtr->pJoiningProcesses, NULL);
}

void InitializeNewProcess(Process *usingProcessPtr, char *name,
                          int (*entryPoint)(void *), void *arg,
                          int stacksize, int priority, int procSlot,
                          int nextPid)
{
    usingProcessPtr->signal = 0;                      // set the signal to an initial value
    usingProcessPtr->cpuTime = 0;                     // set the cpu time to an initial value
    usingProcessPtr->pid = nextPid;                   // set process id
    usingProcessPtr->startTime = 0;                   // set the start time to an initial value
    usingProcessPtr->elapsedTime = 0;                 // set the elapsed time to an initial value
    usingProcessPtr->priority = priority;             // set process priority
    usingProcessPtr->stacksize = stacksize;           // set process stack size
    usingProcessPtr->status = STATUS_READY;           // set the status to ready
    usingProcessPtr->entryPoint = entryPoint;         // set process entry point
    usingProcessPtr->processTableIndex = procSlot;    // set the table index
    usingProcessPtr->quantum = DEFAULT_TIME_SLICE_MS; // set the time slice
    CopyString(name, usingProcessPtr->name, MAXNAME); // Copy process name - shouldn't be NULL
    // Copy process arguments - might be NULL
    if (arg != NULL)
    {
        CopyString(arg, usingProcessPtr->startArgs, MAXARG);
    }
    else
    {
        usingProcessPtr->startArgs[0] = '\0'; // Empty string if no arguments
    }
}

void InitializeProcessTable(Process *usingTablePtr, int size)
{
    int i = 0;
    // loop over the array of nodes and initialize each node to
    // NULL values
    for (i = 0; i < size; i++)
    {
        InitializeProcessToDefault(&usingTablePtr[i]);
    }
}

int GetEmptyControlBlockIndex(Process *fromProcessTablePtr)
{
    int i;

    for (i = 0; i < MAXPROC; i++)
    { // if a pid hasn't been assigned and the context is NULL
        // we have a slot we can use
        if (fromProcessTablePtr[i].context == NULL && fromProcessTablePtr[i].pid == -1)
        {
            return i;
        }
    }

    return -1;
}

Process *GetNextReadyProcess(Process *pRunningProcess,
                             DoublyLinkedList *pPriorityListQueue,
                             int (*pWatchdog)(void *))
{
    Process *pNextProcess = NULL;
    DoublyLinkedNode *pNextLNode = NULL;
    int higherThanPriority = LOWEST_PRIORITY;

    // check the currently running process' priority
    if (pRunningProcess != NULL && pRunningProcess->status == STATUS_RUNNING)
    {
        higherThanPriority = pRunningProcess->priority;
    }

    // check the next ready process' priority
    pNextLNode = pPriorityListQueue[STATUS_READY].pHead;

    // if the next process is NULL return the
    // current running process
    if (pNextLNode == NULL)
    {
        return pRunningProcess;
    }

    // if the next process has a higher or equal priority
    // to the current running process, return the next process
    if (((Process *)pNextLNode->pData)->priority >= higherThanPriority)
    {
        // remove the currently running process from the running list
        // and place it into the ready list if it is still in a running state
        if (pRunningProcess != NULL && pRunningProcess->status == STATUS_RUNNING)
        {

            ChangeProcessStatus(pPriorityListQueue,
                                FindDoublyLinkedNode(pRunningProcess,
                                                     &pPriorityListQueue[STATUS_RUNNING]),
                                STATUS_READY);
        }

        // remove the next ready process from the ready list
        // and place it into the running list
        ChangeProcessStatus(pPriorityListQueue, pNextLNode, STATUS_RUNNING);
        return (Process *)pNextLNode->pData;
    }

    // there is likely a deadlock or the watchdog is the last process to run
    return pWatchdog(NULL);
}

void CleanUpAfterChild(Process *pRunningProcess,
                       DoublyLinkedList *pChildList,
                       DoublyLinkedNode *pStaticStorage,
                       DoublyLinkedList *pPriorityListQueue,
                       int *pCode, int *pResult)
{
    Process *pChild = NULL;
    DoublyLinkedNode *pDynamicNode = NULL;
    DoublyLinkedNode *pStaticNode = NULL;

    // get the first child in the children list
    pDynamicNode = pChildList->pHead;
    // get the pointer to the process from the linked list node
    pChild = (Process *)pDynamicNode->pData;
    // set the exit code
    *pCode = pChild->exitCode;
    // set the pid to the result
    *pResult = pChild->pid;

    // clean up after the child
    pStaticNode = FindStaticStorageNode(pChild->pid, pStaticStorage);
    // Remove the child from the children list
    RemoveDoublyLinkedNode(pChildList, pDynamicNode);
    // Remove the child from the priority quit list
    RemoveDoublyLinkedNode(&pPriorityListQueue[STATUS_QUIT], pStaticNode);
    // Reset the Process Control Block
    InitializeProcessToDefault(pChild);
    // Reset the static linked list node
    InitializeDoublyLinkedNode(pStaticNode);
    // Free the memory for dynamically created node for the parent to track its child
    DestroyDoublyLinkedNode(pDynamicNode);
}
