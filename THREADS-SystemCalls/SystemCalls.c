#define SYSTEM_CALLS_PROJECT
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <THREADSLib.h>
#include <Messaging.h>
#include <Scheduler.h>
#include <DoubleSeaLib.h>
#include <time.h>
#include "_SystemCalls.h"
#include "_Semaphore.h"
#include "UserProcess.h"
#include "libuser.h"

#define DEBUG 0;

/* -------------------------- Globals ------------------------------------- */

static DSL_List semFreeList = {0};				 /* list of free semaphores */
static SemData semTable[MAX_SEMS] = {0};		 /* semaphore table (Static storage) */
static UserProcess userProcTable[MAXPROC] = {0}; /* user process table (Static storage) */

/* ------------------------- Prototypes ----------------------------------- */

int sys_wait(int *pStatus);
void sys_exit(int resultCode);
static void setUserMode(void);
static void setKernelMode(void);
int MessagingEntryPoint(char *);
static void initSystemCallVector(void);
extern int SystemCallsEntryPoint(char *);
static int launchUserProcess(char *pArg);
static void nullsys(system_call_arguments_t *args);
static void checkKernelMode(const char *functionName);
static void sys_call_dispatcher(system_call_arguments_t *args);
int sys_spawn(char *name, int (*startFunc)(char *), char *arg, int stackSize, int priority);
void sys_cputime(int* cpuTime);
void sys_getTimeOfDay(int* tod);

/* ----------------------------- Definitions ---------------------------------- */

int MessagingEntryPoint(char *arg)
{
	int pid;
	int status = -1;

	/* Check for kernel mode */
	checkKernelMode(__func__);

	/* initialize semaphore table */
	InitializeSemTable(semTable, &semFreeList);

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
 * This function is the link between kernel and user mode. It is called by the kernel
 * to start the user process.
 *
 * @param pArg - The argument to the user process.
 *
 * @return int - The result of the user process. *
 */
static int launchUserProcess(char *pArg)
{
	int result = -1;

	/* if signaled when in the sys handler, then Exit */
	if (signaled())
	{
		console_output(FALSE, "%s - Process signaled in launch.\n", "launchUserProcess");
		/* exit */
		sys_exit(result);
		return result;
	}

	/* Set mode to user mode */
	setUserMode();

	/* call the startup function for this process */
	UserProcess *pUserProc = &userProcTable[k_getpid() % MAXPROC];

	result = pUserProc->startFunc(pUserProc->startArgs);

	/* Exit if the startup function returns */
	sys_exit(result);

	return 0; // ?
}

int k_semp(int sem_id)
{
	int result = -1;
	return result;
}

int k_semv(int sem_id)
{
	int result = -1;
	return result;
}

int k_semcreate(int initial_value)
{
	int sem_id = -1;
	return sem_id;
}

int k_semfree(int sem_id)
{
	int result = -1;

	return result;
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
	int result = -1;
	result = k_wait(pStatus);
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

	// create the new process
	pid = k_spawn(name, launchUserProcess, arg, stackSize, priority);
	if (pid < 0)
	{
		console_output(FALSE, "Failed to create user process.");
	}
	else
	{
		/* get the process and its parent from the table */
		int index = pid % MAXPROC;
		UserProcess *pCreatedProcess = &userProcTable[index];
		UserProcess *pParentProcess = &userProcTable[parentPid % MAXPROC];

		/* set the pCreatedProcess data */
		pCreatedProcess->pid = pid;
		pCreatedProcess->status = 1;
		pCreatedProcess->pNext = NULL;
		pCreatedProcess->pPrev = NULL;
		pCreatedProcess->startArgs = arg;
		pCreatedProcess->pNextChild = NULL;
		pCreatedProcess->pPrevChild = NULL;
		pCreatedProcess->tableIndex = index;
		pCreatedProcess->priority = priority;
		pCreatedProcess->startFunc = startFunc;
		pCreatedProcess->pParent = pParentProcess;
		pCreatedProcess->privateMboxId = mailbox_create(0, 0);

		if (pCreatedProcess->privateMboxId < 0)
		{
			console_output(FALSE, "Failed to create mailbox for user process.");
			return -1;
		}

		DSL_InitList(0, OFFSETOF_USER_PROC_CHILD_NODES, &pCreatedProcess->children, NULL);

		/* Set the created process' parent's data */

		/* add the process to the parent's children list
		 * if the parent has no children, then initialize the list to ensure it is ready for use.*/
		if (pParentProcess->children.length <= 0)
		{
			DSL_InitList(0, OFFSETOF_USER_PROC_CHILD_NODES, &pParentProcess->children, NULL);
		}

		DSL_InsertNode(pCreatedProcess, &pParentProcess->children);
	}
	return pid;
}

/**
 * @brief System call wrapper for exiting a process.
 *
 * @param resultCode - The result code of the process.
 */
void sys_exit(int resultCode)
{
	k_exit(resultCode);
	setUserMode();
}

/**
 * @brief System call wrapper getting the CPUTime for the process.
 *
 * @param cpuTime - int pointer to hold the CPU time.
 */
void sys_cputime(int* cpuTime)
{
	/* Use the kernel mode cpu time function */
	/* This might be incorrect because read_time isn't defined in the THREADS spec... */
	*cpuTime = read_time();
}

/**
 * @brief System call wrapper getting the time of day.
 *
 * @param tod - int pointer to time (seconds since epoch)
 */
void sys_getTimeOfDay(int* tod)
{
	/* Not 100% sure if this is correct - cast the return value of time() to an int. */
	*tod = (int)time(NULL);
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
	/* initialize the system call vector */
	for (int i = 0; i < THREADS_MAX_SYSCALLS; i++)
	{
		/* Referring to SystemCalls.h, and the SystemCalls API spec
		we can see that the supported system calls are from 3 to 20
		and range from spawn to getpid.
		*/
		if (i >= SUPPORTED_SYS_CALL_START && i <= SUPPORTED_SYS_CALL_END)
		{
			systemCallVector[i] = sys_call_dispatcher;
		}
		else
		{
			systemCallVector[i] = nullsys;
		}
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
		sys_wait((int *)args->arguments[0]);
		break;
	case SYS_EXIT:
		sys_exit(args->arguments[0]);
		break;
	case SYS_MAILBOX_CREATE:
		break;
	case SYS_MAILBOX_FREE:
		break;
	case SYS_MAILBOX_SEND:
		break;
	case SYS_MAILBOX_RECEIVE:
		break;
	case SYS_SLEEP:
		break;
	case SYS_DISKREAD:
		break;
	case SYS_DISKWRITE:
		break;
	case SYS_DISKSIZE:
		break;
	case SYS_SEMCREATE:
		break;
	case SYS_SEMP:
		break;
	case SYS_SEMV:
		break;
	case SYS_SEMFREE:
		break;
	case SYS_GETTIMEOFDAY:
		sys_getTimeOfDay(args->arguments[0]);
		break;
	case SYS_CPUTIME:
		sys_cputime(args->arguments[0]);
		break;
	case SYS_GETPID:
		break;
	default:
		break;
	}
	/* set mode to user mode before returning.*/
	setUserMode();
}

/*****************************************************************************
   Name - checkKernelMode
   Purpose - Checks the PSR for kernel mode and halts if in user mode
   Parameters -
   Returns -
****************************************************************************/
static inline void checkKernelMode(const char *functionName)
{
	if ((get_psr() & PSR_KERNEL_MODE) == 0)
	{
		console_output(FALSE, "Kernel mode expected, but function called in user mode.\n");
		stop(1);
	}
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
