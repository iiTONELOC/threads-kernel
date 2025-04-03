#define SYSTEM_CALLS_PROJECT
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <THREADSLib.h>
#include <Messaging.h>
#include <Scheduler.h>
#include <DoubleSeaLib.h>
#include "_SystemCalls.h"
#include "_Semaphore.h"
#include "UserProcess.h"
#include "libuser.h"

// #define DEBUG 0;

/* -------------------------- Globals ------------------------------------- */
static DSL_List semFreeList = {0};				 /* list of free semaphores */
static SemData semTable[MAX_SEMS] = {0};		 /* semaphore table (Static storage) */
static UserProcess userProcTable[MAXPROC] = {0}; /* user process table (Static storage) */

/* ------------------------- Prototypes ----------------------------------- */

int sys_wait(int *pStatus);
static void initTables(void);
void sys_exit(int resultCode);
void setUserMode(void);
void sys_cputime(int *cpuTime);
void sys_getTimeOfDay(int *tod);
static void setKernelMode(void);
int MessagingEntryPoint(char *);
static void sys_getPid(int *pid);
static void initSystemCallVector(void);
extern int SystemCallsEntryPoint(char *);
static int launchUserProcess(char *pArg);
static void nullsys(system_call_arguments_t *args);
static void sys_call_dispatcher(system_call_arguments_t *args);
int sys_spawn(char *name, int (*startFunc)(char *), char *arg, int stackSize, int priority);

/* ----------------------------- Definitions ---------------------------------- */

int MessagingEntryPoint(char *arg)
{
	/* --------------------------- KERNEL-SPACE --------------------------- */

	int pid;
	int status = -1;

	/* Check for kernel mode */
	checkKernelMode(__func__);

	/* initialize user process and semaphore tables */
	initTables();

	/* initialize the system call vector */
	initSystemCallVector();

	/* launch the first user process, then wait */
	pid = sys_spawn("SystemCalls", SystemCallsEntryPoint, NULL, THREADS_MIN_STACK_SIZE * 4, 3);

	status = sys_wait(&status);

	return (signaled()) ? (-5) : (0);
} /* MessagingEntryPoint */

/**
 * @brief Launch control for the user process.
 *
 * 		  This function is the link between kernel and user mode.
 *        It is called by the kernel to start the user process.
 *
 * @param pArg - The argument to the user process.
 *
 * @return int - The result of the user process.
 */
static int launchUserProcess(char *pArg)
{
	/* --------------------------- KERNEL-SPACE --------------------------- */

	int result = -1;
	int pid = k_getpid();
	int tableIndex = pid % MAXPROC;

	/* wait for the initialize process to complete.
		 We can't block by disabling interrupts here,
		 so we will use a mailbox to synchronize the
		 process and achieve the same effect.
	 */
	mailbox_receive(userProcTable[tableIndex].privateMboxId, NULL, 0, TRUE);

	/* AFTER UNBLOCK  */
	/* if signaled when in the sys handler, then Exit */
	int _signaled = signaled();
	if (_signaled)
	{
		console_output(FALSE, "%s - Process signaled in launch.\n", "launchUserProcess");
		/* exit */
		sys_exit(result);
		return result;
	}

	/* Set mode to user mode */
	setUserMode();

	/* --------------------------- USER-SPACE --------------------------- */

	/* call the startup function for this process */
	UserProcess *pUserProc = &userProcTable[tableIndex];

	result = pUserProc->startFunc(pUserProc->startArgs);

	/* Exit if the startup function returns
		- Have to use the system call Exit to exit the userland process */
	Exit(result);

	return 0; // ?
}

/**
 * @brief Semaphore P operation.
 *
 * 		  This function decrements the value of
 *        the specified semaphore. If the value is greater than 0, it decrements the value and returns.
 *        If the value is 0, it blocks the calling process until the semaphore is incremented above 0.
 *
 * @param sem_id - The ID of the semaphore to decrement.
 * @return int - The result of the semaphore operation.
 * 			     If successful, returns 0. If an error occurs, returns -1.
 *
 * @note There may be more than one process waiting on a semaphore to be incremented=
 *       If a semaphore is freed, all waiting processes must Exit immediately with error code of 1.
 */
int k_semp(int sem_id)
{
	checkKernelMode(__func__);
	int result = -1;
	if (sem_id < 0 || sem_id >= MAX_SEMS)
	{
		console_output(FALSE, "Error::k_semp: Invalid semaphore ID.\n");
		return result;
	}
	UserProcess *pProcess = &userProcTable[k_getpid() % MAXPROC];
	UserProcEnterCriticalArea(pProcess);
	/* get the semaphore from the semaphore table */
	SemData *pSem = &semTable[sem_id % MAX_SEMS];

	if (pSem->status == SEM_FREE || pSem->status == SEM_INVALID)
	{
		/* check if the semaphore is free or invalid */
		console_output(FALSE, "Error::k_semp: Semaphore not in use.\n");

		if (pSem->status == SEM_FREE)
		{
			// Should not be free here!
		}
		UserProcLeaveCriticalArea(pProcess);
		return result;
	}

	/* check the current value of the semaphore */
	if (pSem->count > 0)
	{
		/* decrement the semaphore count */
		pSem->count--;
		result = 0; // success
	}
	else
	{
		/* Block the current proccess on the semaphore */
		UserProcBlockOnSemaphore(pProcess, pSem);

		/* AFTER the process becomes unblocked */
		if (pSem->status == SEM_FREEING)
		{
			/* check if the process was signaled while waiting */
			pSem->errorOnFree = 1; // set the error code for the semaphore

			UserProcLeaveCriticalArea(pProcess);
			k_exit(1);
			return -1; // error
		}

		/* check if the process was signaled while waiting */
		if (signaled())
		{
			console_output(FALSE, "Error::k_semp: Process was signaled while waiting.\n");
			result = -5; // signaled
		}
		else
		{
			/* semaphore was incremented, decrement the count */
			pSem->count--;
			result = 0; // success
		}
	}

	UserProcLeaveCriticalArea(pProcess);

	return result;
}

/**
 * @brief Semaphore V operation.
 *
 * 		 This function increments the value of the specified semaphore.
 *
 * @param sem_id - The ID of the semaphore to increment.
 * @return int - The result of the semaphore operation. 0 if successful, -1 if an error occurred.
 * @note If there are processes waiting on the semaphore, one of them, will be
 *       unblocked and allowed to continue. The semaphore count is incremented by 1.
 */
int k_semv(int sem_id)
{
	checkKernelMode(__func__);
	int result = -1;

	if (sem_id < 0 || sem_id >= MAX_SEMS)
	{
		console_output(FALSE, "Error::k_semv: Invalid semaphore ID.\n");
		return result;
	}

	UserProcess *pProcess = &userProcTable[k_getpid() % MAXPROC];
	UserProcEnterCriticalArea(pProcess);

	/* get the semaphore from the semaphore table */
	SemData *pSem = &semTable[sem_id % MAX_SEMS];
	if (pSem->status = SEM_FREE || pSem->status == SEM_INVALID)
	{
		console_output(FALSE, "Error::k_semv: Semaphore not in use.\n");
		UserProcLeaveCriticalArea(pProcess);
		return result;
	}

	/* increment the semaphore count */
	pSem->count++;
	result = 0; // success

	/* check if there are any processes waiting on the semaphore */
	if (pSem->count > 0)
	{
		/* unblock one of the waiting processes */
		UserProcess *pWaitingProc = DSL_Pop(&pSem->waitingProcs);
		UserProcUnblockOnSemaphore(pWaitingProc, pSem);

		/* check if the process was signaled while waiting */
		if (signaled())
		{
			console_output(FALSE, "Error::k_semv: Process was signaled while waiting.\n");
			result = -5; // signaled
		}
	}

	UserProcLeaveCriticalArea(pProcess);
	return pSem->errorOnFree ? -1 : result; // return -1 if there was an error on free, else return result
}

/**
 * @brief Kernel level semaphore creation.
 *
 * 		 This function creates a semaphore with the specified initial value.
 *
 * @param initial_value - The initial value of the semaphore.
 * @return int - The semaphore ID if successful, -1 if an error occurred.
 */
int k_semcreate(int initial_value)
{
	checkKernelMode(__func__);
	int sem_id = -1;
	UserProcess *pProcess = &userProcTable[k_getpid() % MAXPROC];
	if (initial_value < 0)
	{
		console_output(FALSE, "Error::k_semcreate: Invalid initial value.\n");
		return -1;
	}

	if (sys_semCount >= MAX_SEMS)
	{
		return -1;
	}

	/* get the next empty semaphore from the semaphore table */
	UserProcEnterCriticalArea(pProcess);
	SemData *pSem = GetNextEmptySemaphore(semTable, MAX_SEMS);
	sem_id = pSem != NULL ? pSem->semId : -1;

	if (sem_id < 0)
	{
		console_output(FALSE, "Error::k_semcreate: No empty semaphores available.\n");
	}
	else
	{
		/* set the value and status */
		pSem->count = initial_value;
		pSem->status = SEM_IN_USE;
	}
	UserProcLeaveCriticalArea(pProcess);
	return sem_id;
}

/**
 * @brief Kernel level semaphore deletion.
 *
 * 		 This function closes the semaphore and unblocks and notifies any waiting processes.
 *
 * @param sem_id - The ID of the semaphore to free.

 * @return int - 0 if successful, 1 if freed - but a process was waiting, -1 if an error occurred when
				decrementing the semaphore.
 */
int k_semfree(int sem_id)
{
	if (sem_id < 0 || sem_id >= MAX_SEMS)
	{
		console_output(FALSE, "Error::k_semfree: Invalid semaphore ID.\n");
		return -1;
	}
	checkKernelMode(__func__);
	int result = -1;

	UserProcess *pProcess = &userProcTable[k_getpid() % MAXPROC];
	UserProcEnterCriticalArea(pProcess);

	/* get the semaphore */
	SemData *pSem = &semTable[sem_id % MAX_SEMS];
	if (pSem->status == SEM_FREE || pSem->status == SEM_INVALID)
	{
		console_output(FALSE, "Error::k_semfree: Semaphore not in use.\n");
		UserProcLeaveCriticalArea(pProcess);
		return result;
	}

	/* set our status to freeing */
	pSem->status = SEM_FREEING;

	/* check if the semaphore has waiting processes */
	if (pSem->waitingProcs.length > 0)
	{
		/* unblock all waiting processes */
		UserProcess *pWaitingProc = NULL;
		while ((pWaitingProc = DSL_Pop(&pSem->waitingProcs)) != NULL)
		{
			/* enter*/
			UserProcUnblockOnSemaphore(pWaitingProc, pSem);
		}
		result = 1; // freed but waiting processes were unblocked
	}
	else
	{
		result = 0; // freed successfully
	}

	/* free the semaphore */
	ResetSem(pSem);
	UserProcLeaveCriticalArea(pProcess);
	return result;
}

static void sys_getPid(int *pid)
{
	/* check for kernel mode */
	checkKernelMode(__func__);

	/* get the process id */
	*pid = k_getpid();
}

/**
 * @brief System call wrapper for waiting for a process to complete.
 *
 * @param pStatus - Pointer to an int that receives the exit code of the process
 *
 * @return The pid of the process that exited
 * @return -1 if there are no child processes to wait for
 * @return -5 if the process was signalled while waiting
 */
int sys_wait(int *pStatus)
{
	/* check for kernel mode */
	checkKernelMode(__func__);
	int result = -1;

	/* wait for the child process to exit
		k_wait returns the pid of the process that exited or -1, or -5 if signaled
	*/
	result = k_wait(pStatus);

	/* Error occurred - TODO: Handle Errors */
	if (result < 0)
	{
		if (result == -1)
		{
			console_output(FALSE, "Error::sys_wait: No child processes to wait for.\n");
		}
		else if (result == -5)
		{
			console_output(FALSE, "Error::sys_wait: Process was signaled while waiting.\n");
		}
		else
		{
			console_output(FALSE, "Error::sys_wait: Unknown error occurred in sys_wait.\n");
		}
		return result;
	}

	/* get the process that exited */
	UserProcess *pExitingChild = &userProcTable[result % MAXPROC];

	/* see if the exiting process has a parent, if so remove it from its parent's list */
	UserProcEnterCriticalArea(pExitingChild);
	if (pExitingChild->pParent != NULL)
	{
		/* remove this process from the list of children */
		DSL_RemoveNode(pExitingChild, &pExitingChild->pParent->children);
	}
	UserProcLeaveCriticalArea(pExitingChild);

	/*
		TODO:?? Do we need to check if the process exists in a wait list for a semaphore?
	*/

	return (signaled()) ? (-5) : (result);
}

/**
 * @brief System call wrapper for spawning a new process.
 *
 * @param name - The name of the process.
 * @param startFunc - The function to start the process.
 * @param arg - The argument to the function.
 * @param stackSize - The stack size for the process.
 * @param priority - The priority of the process.
 */
int sys_spawn(char *name, int (*startFunc)(char *), char *arg, int stackSize, int priority)
{
	/* check for kernel mode */
	checkKernelMode(__func__);
	int parentPid;
	int pid = -1;

	/* validate the parameters */
	if (!name || !startFunc || stackSize < THREADS_MIN_STACK_SIZE || priority < 0)
	{
		console_output(FALSE, "Invalid parameters for sys_spawn.\n");
		return -1;
	}

	/* we are the parent*/
	parentPid = k_getpid();

	/* create the new process */
	pid = k_spawn(name, launchUserProcess, arg, stackSize, priority);
	if (pid > 0)
	{
		int index = pid % MAXPROC;
		UserProcess *pCreatedProcess = &userProcTable[index];

		ResuseProcArgs args = {
			.arg = arg,
			.pid = pid,
			.name = name,
			.priority = priority,
			.parentPid = parentPid,
			.startFunc = startFunc,
			.stackSize = stackSize,
			.pUserProcTable = userProcTable,
		};

		ReuseUserProcess(&args);

		/* send a message to the process to start - don't block*/
		mailbox_send(pCreatedProcess->privateMboxId, NULL, 0, FALSE);
	}
	return pid;
}

/**
 * @brief Initializes the semaphore and user process tables.
 */
static void initTables(void)
{
	/* loop over the semaphore table and init the semaphore and user processes
		by attaching mailboxes to them.
		We loop MAX_SEMS(n) instead of (MAX_SEMS + MAXPROC)(n)
	*/
	for (int i = 0; i < MAX_SEMS; ++i)
	{
		/* if we are less than the max processes, handle user processes as well */
		if (i < MAXPROC)
		{
			userProcTable[i].tableIndex = i;
			/* add a mailbox to the user process table */
			userProcTable[i].privateMboxId = mailbox_create(1, sizeof(int));
			userProcTable[i].semWaitMboxId = mailbox_create(1, sizeof(int));
			/* remaining process fields are NULL for now -
				they will be set when a process is spawned */
		}

		/* initialize the semaphore table w/defaults */
		InitializeSemWData(&semTable[i], -1, 0, SEM_FREE);
	}
}

/**
 * @brief System call wrapper for exiting a process.
 *
 * @param resultCode - The result code of the process.
 */
void sys_exit(int resultCode)
{
	/* check for kernel mode */
	checkKernelMode(__func__);

	int exitCode = 0;
	UserProcess *pChild;
	int pid = k_getpid();
	int tableIndex = pid % MAXPROC;
	UserProcess *pProcess = &userProcTable[tableIndex];

	/* Check for children */
	UserProcEnterCriticalArea(pProcess);
	while ((pChild = DSL_Pop(&pProcess->children)) != NULL)
	{
		/* if the child is still alive */
		if (pChild->status == USER_PROC_IN_USE)
		{
			UserProcLeaveCriticalArea(pProcess);
			/* Send the kill signal to the child */
			k_kill(((UserProcess *)pChild)->pid, SIG_TERM);
			/* wait for it to exit */
			k_wait(&exitCode);
			UserProcEnterCriticalArea(pProcess);
		}
	}

	/* call k_exit to terminate the process */
	pProcess->status = USER_PROC_FREE;
	UserProcLeaveCriticalArea(pProcess);

	k_exit(resultCode);
}

/**
 * @brief System call wrapper getting the CPUTime for the process.
 *
 * @param cpuTime - int pointer to hold the CPU time.
 */
void sys_cputime(int *cpuTime)
{
	/* check for kernel mode */
	checkKernelMode(__func__);
	/* Use the kernel mode cpu time function */
	*cpuTime = read_time();
}

/**
 * @brief System call wrapper getting the time of day.
 *
 * @param tod - int pointer to time (seconds since epoch)
 */
void sys_getTimeOfDay(int *tod)
{
	/* check for kernel mode */
	checkKernelMode(__func__);
	*tod = system_clock();
}

/**
 * @brief Initializes the system call vector.
 *
 * This function initializes the system call vector with the appropriate system call handlers.
 *
 * @return void
 */
static void initSystemCallVector(void)
{

	for (int i = SUPPORTED_SYS_CALL_START; i <= SUPPORTED_SYS_CALL_END; i++)
	{
		systemCallVector[i] = sys_call_dispatcher;
	}
}

/**
 * @brief System call dispatcher.
 * This function is called by the system call interrupt handler to dispatch
 * the appropriate system call handler.
 *
 * @param args - The system call arguments.
 * @return void
 */
static void sys_call_dispatcher(system_call_arguments_t *args)
{
	/* check for kernel mode */
	checkKernelMode(__func__);

	/* --------------------------- KERNEL-SPACE --------------------------- */

	int result = -1;

	/* Check for valid system call arguments */
	if (!args || args->call_id < 0 || args->call_id >= THREADS_MAX_SYSCALLS)
	{
		console_output(FALSE, "Invalid system call arguments.\n");
		return result;
	}

	/* ensure we are in kernel mode */
	setKernelMode();

	// Call the appropriate system call handler
	switch (args->call_id)
	{
	case SYS_SPAWN:
		/*spawn a process using sys_spawn*/
		result = sys_spawn((char *)args->arguments[4], (int (*)(char *))args->arguments[0],
						   (char *)args->arguments[1], (int)args->arguments[2], (int)args->arguments[3]);

		/* set the expected return values */
		args->arguments[0] = result;
		args->arguments[3] = (result > 0) ? (0) : (-1);
		break;
	case SYS_WAIT:
		result = sys_wait((int *)&args->arguments[1]);
		/* set the expected return values */
		args->arguments[3] = args->arguments[0];
		args->arguments[0] = result;

		break;
	case SYS_EXIT:
		sys_exit(args->arguments[0]);
		break;
	case SYS_SLEEP:
		// TODO: Implement sleep
		break;
	case SYS_SEMCREATE:
		/* create a semaphore using sys_semcreate */
		result = k_semcreate((int)args->arguments[0]);
		args->arguments[0] = result;
		args->arguments[3] = (result >= 0) ? (0) : (-1);
		break;
	case SYS_SEMP:
		/* P operation on a semaphore using sys_semp */
		result = k_semp((int)args->arguments[0]);
		args->arguments[3] = (result >= 0) ? (0) : (-1);
		break;
	case SYS_SEMV:
		/* V operation on a semaphore using sys_semv */
		result = k_semv((int)args->arguments[0]);
		args->arguments[3] = (result >= 0) ? (0) : (-1);
		break;
	case SYS_SEMFREE:
		result = k_semfree((int)args->arguments[0]);
		args->arguments[3] = result;
		break;
	case SYS_GETTIMEOFDAY:
		sys_getTimeOfDay((int *)&args->arguments[0]);
		break;
	case SYS_CPUTIME:
		sys_cputime((int *)&args->arguments[0]);
		break;
	case SYS_GETPID:
		sys_getPid((int *)&args->arguments[0]);
		break;
	default:
		/* invalid syscall - call the nullsys handler */
		nullsys(args);
		break;
	}
	/* set mode to user mode before returning.*/
	setUserMode();
	/* --------------------------- USER-SPACE --------------------------- */
}

/* an error method to handle invalid syscalls */
static void nullsys(system_call_arguments_t *args)
{
	console_output(FALSE, "nullsys(): Invalid syscall %d. Halting...\n", args->call_id);
	stop(1);
} /* nullsys */

/**
 * @brief Sets the processor mode to user mode.
 *
 * This function sets the processor mode to user mode using bitwise operations.
 *
 * Check C Primer Plus pp. 679 - 683 for more information on bitwise operations.
 */
static void setUserMode(void)
{
	/* the Kernel mode bit is in bit position 1 and we can use the PSR_KERNEL_MODE for the mask
	   a Value of 0 in the Kernel mode bit position will set the processor to user mode */
	set_psr(get_psr() & ~PSR_KERNEL_MODE);
}

/**
 * @brief Sets the processor mode to kernel mode.
 *
 * This function sets the processor mode to kernel mode using bitwise operations.
 *
 * Check C Primer Plus pp. 679 - 683 for more information on bitwise operations.
 */
static void setKernelMode(void)
{
	/* the Kernel mode bit is in bit position 1 and we can use the PSR_KERNEL_MODE for the mask
	   a Value of 1 in the Kernel mode bit position will set the processor to kernel mode */
	set_psr(get_psr() | PSR_KERNEL_MODE);
}
