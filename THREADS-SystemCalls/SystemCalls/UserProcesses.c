#include "UserProcess.h"

/* -------------------------------------- Definitions ----------------------------------- */

/**
 * @brief Checks if the current process is in kernel mode.
 *
 * @param functionName - The name of the function that called this function.
 * @note This function checks the PSR register to determine if the current process is in kernel mode.
 *       If not, it prints an error message and stops the execution.
 */
void checkKernelMode(const char *functionName)
{
    if (functionName == NULL)
    {
        return; /* Invalid function name */
    }

    if ((get_psr() & PSR_KERNEL_MODE) == 0)
    {
        console_output(FALSE, "Kernel mode expected, but function called in user mode.\n");
        stop(1);
    }
}

/**
 * @brief Resets the user process to its initial state.
 *
 * @param pUserProc - Pointer to the user process to reset.
 * @note this does not reset the tableIndex
 */
void ResetUserProcess(UserProcess *pUserProc)
{
    if (pUserProc == NULL)
    {
        return; /* Invalid user process pointer */
    }

    pUserProc->pid = 0;
    pUserProc->status = 0;
    pUserProc->priority = 0;
    pUserProc->pNext = NULL;
    pUserProc->pPrev = NULL;
    pUserProc->pParent = NULL;
    memset(pUserProc->startArgs, 0, MAX_START_ARG_LEN);
    pUserProc->startFunc = NULL;
    pUserProc->pNextChild = NULL;
    pUserProc->pPrevChild = NULL;

    /* destroy list - do not delete any list nodes from memory*/
    DSL_DestroyList(&pUserProc->children, 0);
    /* Re-init the LL */
    DSL_InitList(0, OFFSETOF_USER_PROC_CHILD_NODES, &pUserProc->children, NULL);
}

/**
 * @brief Marks the process as outside its critical section using system mailboxes.
 *
 *  This is used to synchronize the process with the kernel.
 *
 * @param pProcess - The process to enable interrupts for.
 *
 * @note Uses a single-slot mailbox to synchronize with the kernel.
 */
void userProcLeaveCriticalArea(UserProcess *pProcess)
{
    if (!pProcess)
    {
        return; /* Invalid process pointer */
    }
    /* check for kernel mode */
    checkKernelMode(__func__);
    mailbox_receive(pProcess->privateMboxId, NULL, 0, TRUE);
}

/**
 * @brief Marks the process as being inside its critical section using system mailboxes.
 *
 *  This is used to synchronize the process with the kernel.
 *
 * @param pProcess - The process to disable interrupts for.
 *
 * @note Uses a single-slot mailbox to synchronize with the kernel.
 */
void userProcEnterCriticalArea(UserProcess *pProcess)
{
    if (!pProcess)
    {
        return; /* Invalid process pointer */
    }
    /* check for kernel mode */
    checkKernelMode(__func__);
    mailbox_send(pProcess->privateMboxId, NULL, 0, TRUE);
}

/**
 * @brief Blocks the process on a semaphore.
 *
 *  This function blocks the process on a semaphore until it is unblocked.
 *
 * @param pProcess - The process to block on the semaphore.
 * @param pSem - The semaphore to block on.
 */
void userProcBlockOnSemaphore(UserProcess *pProcess, SemData *pSem)
{
    if (!pSem || !pProcess)
    {
        return; /* Invalid semaphore or process pointer */
    }
    /* check for kernel mode */
    checkKernelMode(__func__);

    /* set the status of the sem to waiting */
    pSem->status = SEM_WAITING;
    /* block the process until the semaphore is incremented */
    pProcess->status = USER_PROC_WAITING;
    /* add the process to the waiting list */
    DSL_InsertNode(pProcess, &pSem->waitingProcs);
    /* wait for the semaphore to be incremented */
    mailbox_receive(pProcess->semWaitMboxId, NULL, 0, TRUE);
}

/**
 * @brief Unblocks the process on a semaphore.
 *
 *  This function unblocks the process on a semaphore.
 *
 * @param pProcess - The process to unblock on the semaphore.
 * @param pSem - The semaphore to unblock on.
 */
void userProcUnblockOnSemaphore(UserProcess *pProcess, SemData *pSem)
{
    if (!pSem || !pProcess)
    {
        return; /* Invalid semaphore or process pointer */
    }

    /* check for kernel mode */
    checkKernelMode(__func__);

    /* update status */
    pProcess->status = USER_PROC_IN_USE;

    /* if there are no more waiting processes set the sems status */
    if (pSem->waitingProcs.length == 0)
    {
        pSem->status = SEM_IN_USE;
    }

    /* send a message to the process to unblock it */
    mailbox_send(pProcess->semWaitMboxId, NULL, 0, TRUE);
}

/**
 * @brief Creates a new process in the user process table.
 *
 * @param pArgs - Pointer to the arguments for creating the process.
 * @param pArgs->pid - Process ID of the new process.
 * @param pArgs->parentPid - Parent process ID.
 * @param pArgs->name - Name of the new process.
 * @param pArgs->startFunc - Pointer to the function to execute in the new process.
 * @param pArgs->arg - Argument to pass to the function.
 * @param pArgs->stackSize - Size of the stack for the new process.
 * @param pArgs->priority - Priority of the new process.
 * @param pArgs->pUserProcTable - Pointer to the user process table.

 * @note This function initializes the user process data structure the provided values.
 */
void ReuseUserProcess(ResuseProcArgs *pArgs)
{
    if (!pArgs || !pArgs->pUserProcTable)
    {
        return; /* Invalid arguments pointer */
    }

    /* check for kernel mode */
    checkKernelMode(__func__);

    int index = pArgs->pid % MAX_PROCESSES;
    UserProcess *pUserProc = &pArgs->pUserProcTable[index];
    UserProcess *pParentProc = &pArgs->pUserProcTable[pArgs->parentPid % MAX_PROCESSES];

    /* set the process data */
    pUserProc->pNext = NULL;
    pUserProc->pPrev = NULL;
    pUserProc->pid = pArgs->pid;
    pUserProc->pNextChild = NULL;
    pUserProc->pPrevChild = NULL;
    pUserProc->pParent = pParentProc;
    pUserProc->status = USER_PROC_IN_USE;
    pUserProc->priority = pArgs->priority;
    pUserProc->startFunc = pArgs->startFunc;
    if (pArgs->arg != NULL)
    {
        strncpy(pUserProc->startArgs, pArgs->arg, MAX_START_ARG_LEN);
    }
    DSL_InitList(0, OFFSETOF_USER_PROC_CHILD_NODES, &pUserProc->children, NULL);

    /* set the parent process data */
    if (pParentProc != NULL)
    {
        DSL_InsertNode(pUserProc, &pParentProc->children);
    }
    else
    {
        console_output(FALSE, "Error::ReuseUserProcess: Parent process is NULL.\n");
    }
}