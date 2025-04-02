#pragma once
#ifndef _USERPROCESS_H
#define _USERPROCESS_H
#include <THREADSLib.h>
#include <Messaging.h>
#include "_SystemCalls.h"

typedef struct resuse_proc_args
{
    int pid;
    char *arg;
    char *name;
    int priority;
    int stackSize;
    int parentPid;
    int (*startFunc)(char *);
    UserProcess *pUserProcTable;
} ResuseProcArgs;

/* -------------------------------- Function Prototypes ------------------------------- */

/**
 * @brief Checks if the current process is in kernel mode.
 *
 * @param functionName - The name of the function that called this function.
 * @note This function checks the PSR register to determine if the current process is in kernel mode.
 *       If not, it prints an error message and stops the execution.
 */
void checkKernelMode(const char *functionName);

/**
 * @brief Resets the user process to its initial state.
 *
 * @param pUserProc -  Pointer to the user process to reset.
 * @note this does not reset the tableIndex
 */
void ResetUserProcess(UserProcess *pUserProc);

/**
 * @brief Marks the process as outside its critical section using system mailboxes.
 *
 *  This is used to synchronize the process with the kernel.
 *
 * @param pProcess - The process to enable interrupts for.
 *
 * @note Uses a single-slot mailbox to synchronize with the kernel.
 */
void userProcLeaveCriticalArea(UserProcess *pProcess);

/**
 * @brief Marks the process as being inside its critical section using system mailboxes.
 *
 *  This is used to synchronize the process with the kernel.
 *
 * @param pProcess - The process to disable interrupts for.
 *
 * @note Uses a single-slot mailbox to synchronize with the kernel.
 */
void userProcEnterCriticalArea(UserProcess *pProcess);

/**
 * @brief Blocks the process on a semaphore.
 *
 *  This function blocks the process on a semaphore until it is unblocked.
 *
 * @param pProcess - The process to block on the semaphore.
 * @param pSem - The semaphore to block on.
 */
void userProcBlockOnSemaphore(UserProcess *pProcess, SemData *pSem);

/**
 * @brief Unblocks the process on a semaphore.
 *
 *  This function unblocks the process on a semaphore.
 *
 * @param pProcess - The process to unblock on the semaphore.
 * @param pSem - The semaphore to unblock on.
 */
void userProcUnblockOnSemaphore(UserProcess *pProcess, SemData *pSem);

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
void ReuseUserProcess(ResuseProcArgs *pArgs);

#endif
