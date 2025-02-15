#include "SchedulerHelpers.h"

/**
 * @brief Increments the process id
 *1
 * This function increments the process id and sets the nextPid to the new value.
 */
void IncrementPid()
{
    if (nextPid % MAX_PROCESSES == 0)
    {
        // This gives us the same output that the test program expects
        // In reality, we can just increment the id, we save its index in the PCB
        // If you have access to the process struct, you can get its index into
        // the table AND linked list node static storage
        nextPid = nextPid + 3;
    }
    else
    {
        nextPid++;
    }
}

/**
 * @brief Initialize the ready list
 *
 * This function initializes the ready list by calling the InitializeProcessList function
 * for each priority level.
 */
void SchedulerInitReadyList()
{
    for (int i = 0; i < NUM_PRIORITIES; i++)
    {
        InitializeProcessList(&readyList[i], READY_PROCESSES_LIST);
    }
}

/**
 * @brief Convert a process id to an index
 *
 * This function converts a process id to an index in the process table.
 *
 * @param pid The process id to convert
 * @param pIndex A pointer to the index to set
 */
int SchedulerPidToIndex(int pid)
{
    return (pid % MAX_PROCESSES) - 1;
}

/**
 * @brief Get the next process to run
 *
 * @return Process* Pointer to the next process to run
 */
Process *SchedulerGetNextProcess()
{
    Process *nextProcess = NULL;
    int currentPriority = LOWEST_PRIORITY;

    // if the running process is not null and its status is running
    // use a priority floor when getting the next process to run
    if (runningProcess != NULL && runningProcess->status == STATUS_RUNNING)
    {
        currentPriority = runningProcess->priority;
    }

    for (int i = HIGHEST_PRIORITY; i >= currentPriority; i--)
    {
        if (readyList[i].count > 0)
        {
            nextProcess = PopProcessFromList(&readyList[i]);

            break;
        }
    }

    return nextProcess;
}

/**
 * @brief Cleans up an exited process
 *
 * This function cleans up a process by stopping its context and resetting the master list entry
 * and resetting the PCB.
 *
 * @param pProcess A pointer to the process to clean up
 */
void SchedulerCleanUpProcess(Process *pProcess)
{
    context_stop(pProcess->context);
    // Reset the child process in the process table
    memset(pProcess, 0, sizeof(Process));
    // Decrement the process count
    processCount--;
}

/**
 * @brief Create a new process
 *
 * @param pProps A pointer to the properties for the new process
 */
void SchedulerCreateNewProcess(NewProcessArgs *pProps)
{
    // Create a new process in the control block
    CreateNewProcess(pProps);

    /* Initialize the Process's Master List Entry*/
    for (int i = 0; i < NUM_UNIQUE_LISTS; i++)
    {
        InitializeProcessList(&linkedListMaster[pProps->procSlot][i], i + LIST_TYPE_TO_PROC_MASTER_OFFSET);
    }

    /* If there is a parent process,add this to the list of children. */
    if (runningProcess != NULL)
    {
        AddProcessToList(pProps->pNewProcess,
                         &linkedListMaster[runningProcess->processTableIndex][GetProcessListIndex(PROCESS_CHILDREN_LIST)]);
        pProps->pNewProcess->pParent = runningProcess;
    }

    /* Add the process to the ready list. */
    AddProcessToList(pProps->pNewProcess, &readyList[pProps->priority]);

    /* Increment the PID for the next process*/
    IncrementPid();

    /* Increment the process count */
    processCount++;
}

/**
 * @brief Handle the context switch for the scheduler
 *
 * @param pNextProcess A pointer to the next process to run
 * @note This function is only called when the scheduler is handling the context switch.
 *       Checks already happened to ensure the current running process (if any) needs
 *       to be preempted.
 */
void SchedulerHandleContextSwitch(Process *pNextProcess)
{
    // if the running process exists and is currently running
    if (runningProcess != NULL && runningProcess->status == STATUS_RUNNING)
    { // move it to the ready list
        runningProcess->status = STATUS_READY;
        AddProcessToList(runningProcess, &readyList[runningProcess->priority]);
    }

    // set the next process to run
    runningProcess = pNextProcess;
    runningProcess->status = STATUS_RUNNING;
    context_switch(runningProcess->context);
}

// ________________________________ Process Helpers ________________________________

void PrintProcessRow(Process *pProcess)
{
    char *statusStr = NULL;
    char statusBuffer[20] = {0};

    // Grab the Status String
    if (pProcess->status < NUM_PROCESS_STATES)
    {
        statusStr = STATUS_STRINGS[pProcess->status];
    }
    else
    {
        // Use the User Defined Number
        snprintf(statusBuffer, sizeof(statusBuffer), "%d", pProcess->status);
        statusStr = statusBuffer;
    }

    // Print process information
    console_output(0, PROCESS_TABLE_ROW_FORMAT,
                   pProcess->pid,
                   pProcess->pParent == NULL ? -1 : pProcess->pParent->pid,
                   pProcess->priority,
                   statusStr,
                   linkedListMaster[pProcess->processTableIndex][GetProcessListIndex(PROCESS_CHILDREN_LIST)].count,
                   pProcess->cpuTime,
                   pProcess->name);
}

void PrintProcessTable(Process *usingTablePtr, int size)
{
    int i = 0;

    // Print header
    console_output(0, PROCESS_TABLE_HEADER_FORMAT,
                   "PID",
                   "Parent",
                   "Priority",
                   "Status",
                   "# Kids",
                   "CPUtime",
                   "Name");

    // if the table is full print the last process first
    if (processCount == MAXPROC)
    {
        PrintProcessRow(&usingTablePtr[MAXPROC - 1]);
        // adjust the size to print the rest of the table
        size--;
    }

    // Loop through process table
    for (i; i < size; i++)
    {
        Process *pProcess = &usingTablePtr[i];
        // Skip empty process slots
        if (pProcess->context == NULL)
        {
            continue;
        }

        // Print process row
        PrintProcessRow(pProcess);
    }
}
/**
 * @brief Handle zombie children
 *
 * This function handles zombie children by checking if the process has any children
 * and if they are in the zombie state. If they are, the process is cleaned up.
 *
 * @param pProcess A pointer to the process to check for zombie children
 */
int HandleZombieChildren(Process *pProcess, int *pExitCode)
{
    int result = -1500;
    Process *pChildProcess = NULL;
    LinkedProcessList *pRunningProcessZombieChildrenList =
        &linkedListMaster[runningProcess->processTableIndex][GetProcessListIndex(PROCESS_ZOMBIE_CHILDREN_LIST)];

    // if there are zombie children, return the first one
    if (pRunningProcessZombieChildrenList->count > 0)
    {
        pChildProcess = PopProcessFromList(pRunningProcessZombieChildrenList);
        if (pChildProcess != NULL)
        {
            CleanUpChildProcess(pChildProcess, pExitCode, &result);
            return result;
        }
    }

    return result;
}

/**
 * @brief Handle exiting children
 *
 * This function handles exiting children by checking if the process has any children
 * and if they are in the exiting state. If they are, the process is cleaned up.
 *
 * @param pProcess A pointer to the process to check for exiting children
 */
int HandleExitingChildren(Process *pProcess, int *pExitCode)
{
    int result = -1500;
    Process *pChildProcess = NULL;
    LinkedProcessList *pRunningProcessExitingChildrenList =
        &linkedListMaster[runningProcess->processTableIndex][GetProcessListIndex(PROCESS_EXITING_CHILDREN_LIST)];

    pChildProcess = PopProcessFromList(pRunningProcessExitingChildrenList);
    if (pChildProcess != NULL)
    {
        CleanUpChildProcess(pChildProcess, pExitCode, &result);
    }
    else
    {
        console_output(FALSE, "k_wait(): No child process found in the exiting children list\n");
    }

    return result;
}

/**
 * @brief Cleans up a child process
 *
 * @param pProcess A pointer to the process to clean up
 * @param pChildExitCode A pointer to the child's exit code
 * @param pResult A pointer to the result of the cleanup
 */
void CleanUpChildProcess(Process *pProcess, int *pChildExitCode, int *pResult)
{
    *pResult = pProcess->pid;
    *pChildExitCode = pProcess->exitCode;

    SchedulerCleanUpProcess(pProcess);
}
