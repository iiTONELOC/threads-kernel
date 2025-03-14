#include "THREADSLib.h"
#include "Processes.h"
#include "SchedulerUtils.h"
#include "PriorityProcessQueue.h"

/*_______________________Function  Definitions_______________________*/

/**
 * @brief Order function for the test data.
 *
 * @param pNode1 The first process to compare.
 * @param pNode2 The second process to compare.
 *
 * @return The difference between the two priorites.
 */
int orderFunction(void *pNode1, void *pNode2)
{
    if (pNode1 == NULL || pNode2 == NULL)
    {
        return 0;
    }

    Process *process1 = (Process *)((DSL_Node *)pNode1)->pData;
    Process *process2 = (Process *)((DSL_Node *)pNode2)->pData;

    if (process1 == NULL || process2 == NULL)
    {
        return 0;
    }

    // descending order, the linked list test runs ascending order
    // so here we cover both bases as this function is passed to the
    // linked list's initialization function
    return process2->priority - process1->priority;
}

/**
 * @brief Initializes a process to Default values
 *
 * @param usingProcessPtr The node to initialize
 *
 * @note  Pointers are set to NULL, unsigned integers are set to 0, and signed
 *        integers are set to -1. The LinkedLists are initialized. *
 */
void InitializeProcessToDefault(Process *usingProcessPtr)
{
    usingProcessPtr->pid = -1;
    usingProcessPtr->status = -1;
    usingProcessPtr->signal = -1;
    usingProcessPtr->cpuTime = 0;
    usingProcessPtr->stack = NULL;
    usingProcessPtr->exitCode = -1;
    usingProcessPtr->priority = -1;
    usingProcessPtr->startTime = -1;
    usingProcessPtr->stacksize = -1;
    usingProcessPtr->context = NULL;
    usingProcessPtr->pParent = NULL;
    usingProcessPtr->joinStatus = -99;
    usingProcessPtr->elapsedTime = -1;
    usingProcessPtr->entryPoint = NULL;
    usingProcessPtr->processTableIndex = -1;
    usingProcessPtr->quantum = MAX_PROC_QUANTUM;
    DSL_InitList(FALSE, OFFSETOF_DSL_NODE, &usingProcessPtr->pChildren, orderFunction);
    DSL_InitList(FALSE, OFFSETOF_DSL_NODE, &usingProcessPtr->pDeadChildren, orderFunction);
    DSL_InitList(FALSE, OFFSETOF_DSL_NODE, &usingProcessPtr->pExitingChildren, orderFunction);
    DSL_InitList(FALSE, OFFSETOF_DSL_NODE, &usingProcessPtr->pJoiningProcesses, orderFunction);
}

/**
 * @brief Initialize a new process
 *
 * @param usingProcessPtr The process to initialize
 * @param name The name of the process
 * @param entryPoint The entry point of the process
 * @param arg The arguments to pass to the process
 * @param stacksize The size of the stack
 * @param priority The priority of the process
 * @param procSlot The slot in the process table
 * @param nextPid The next process id
 */
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
    usingProcessPtr->joinStatus = -99;                // explicitly reset the join
    usingProcessPtr->priority = priority;             // set process priority
    usingProcessPtr->stacksize = stacksize;           // set process stack size
    usingProcessPtr->status = STATUS_READY;           // set the status to ready
    usingProcessPtr->entryPoint = entryPoint;         // set process entry point
    usingProcessPtr->processTableIndex = procSlot;    // set the table index
    usingProcessPtr->quantum = MAX_PROC_QUANTUM;      // set the time slice
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

/**
 * @brief Clean up after exited children
 *
 * @param pRunningProcess Pointer to the currently running process
 * @param pChildList Pointer to the list of children
 * @param pStaticStorage Pointer to the static storage array
 * @param pPriorityListQueue Pointer to the priority list queue
 * @param pCode Pointer to the exit code
 * @param pResult Pointer to the result
 */
void CleanUpAfterChild(Process *pRunningProcess,
                       DSL_List *pChildList,
                       DSL_Node *pStaticStorage,
                       DSL_List *pPriorityListQueue,
                       int *pCode, int *pResult)
{
    Process *pChild = NULL;
    DSL_Node *pStaticNode = NULL;
    DSL_Node *pDynamicNode = NULL;

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
    DSL_RemoveNode(pDynamicNode, pChildList);
    // Remove the child from the priority quit list
    DSL_RemoveNode(pStaticNode, &pPriorityListQueue[STATUS_QUIT]);
    // Clean up the child process
    CleanUpPCB(pChild, pStaticNode);
    // -----------------------------------
    // Free the memory for dynamically created node for the parent to track its child
    DestroyDoublyLinkedNode(pDynamicNode);
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
    { // if a pid hasn't been assigned and the context is NULL
        // we have a slot we can use
        if (fromProcessTablePtr[i].context == NULL && fromProcessTablePtr[i].pid == -1)
        {
            return i;
        }
    }

    return -1;
}
/**
 * @brief Initializes the processes Table
 *
 * Takes an array of Processes and initializes all values to NULL
 *
 * @param usingTablePtr A pointer to the process table to initialize
 * @param size The size of the process table
 */
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

/**
 * @brief Clean up after a process with no parent or children
 *
 * Removes the process context and initializes the PCB to default values
 *
 * @param pProcessToClean Pointer to the process to clean up
 * @param pStaticStorageNode Pointer to the static storage node
 */
void CleanUpPCB(Process *pProcessToClean, DSL_Node *pStaticStorageNode)
{
    context_stop(pProcessToClean->context);
    InitializeProcessToDefault(pProcessToClean);
    DSL_InitNode(FALSE, pStaticStorageNode, NULL);
}

/**
 * @brief Get the next ready process from the READY queue
 *
 * @param pRunningProcess Pointer to the currently running process
 * @param pPriorityListQueue Pointer to the priority list queue
 *
 * @return Pointer to the next ready process or NULL if there are none
 */
Process *GetNextReadyProcess(Process *pRunningProcess,
                             DSL_List *pPriorityListQueue)
{
    Process *pNextProcess = NULL;
    DSL_Node *pNextLNode = NULL;
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
    if (pNextLNode == NULL && pRunningProcess != NULL)
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
                                (DSL_Node *)*DSL_FindNode(
                                    &pPriorityListQueue[STATUS_RUNNING], (void *)pRunningProcess),
                                STATUS_READY);
        }

        // remove the next ready process from the ready list
        // and place it into the running list
        ChangeProcessStatus(pPriorityListQueue, pNextLNode, STATUS_RUNNING);
        return (Process *)pNextLNode->pData;
    }

    return NULL;
}
