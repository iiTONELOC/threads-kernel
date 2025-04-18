#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <THREADSLib.h>
#include <Messaging.h>
#include <Scheduler.h>
#include <libuser.h>
#include <SystemCalls.h>
#include <_Devices.h>

static int running;															   /*semaphore to synchronize drivers and start3*/
interrupt_handler_t *handlers;												   /*From THREADS*/
DiskInformation diskInformation[THREADS_MAX_DISKS] = {0};					   /* disk information structure */
static DevicesProcess DevicesProcesss[MAXPROC] = {0};						   /* device process table (Static storage) */
void (*systemCallVector[THREADS_MAX_SYSCALLS])(system_call_arguments_t *args); /* system call array of function pointers */

static void setUserMode(void);
static int DiskDriver(char *);
static int ClockDriver(char *);
static void setKernelMode(void);
static void InitializeHandlers(void);
extern int DevicesEntryPoint(char *);
static inline void enableInterrupts();
static inline void checkKernelMode(const char *functionName);
static void sys_call_dispatcher(system_call_arguments_t *args);
static int GetDiskInfo(int unit, DiskInformation *diskInfo, DeviceControl *devRequest);

int SystemCallsEntryPoint(char *arg)
{
	int i;
	int status;
	char buf[25];
	char name[128];
	int clockPID = 0;
	int diskPids[THREADS_MAX_DISKS];

	checkKernelMode(__func__);

	/* Assign system call handlers */
	InitializeHandlers();

	// /* Initialize the process table */
	// for (int i = 0; i < MAXPROC; ++i)
	// {
	// }

	/*
	 * Create clock device driver
	 * Use a semaphore or mailbox for coordinating the start of the drive
	 */
	running = k_semcreate(0);
	clockPID = k_spawn("Clock driver", ClockDriver, NULL, THREADS_MIN_STACK_SIZE, HIGHEST_PRIORITY);
	if (clockPID < 0)
	{
		console_output(TRUE, "start3(): Can't create clock driver\n");
		stop(1);
	}

	/*
	 * Wait for the clock driver to start. The idea is that ClockDriver
	 * will V the semaphore "running" once it is running.
	 */
	k_semp(running);

	/*
	 * Create the disk device drivers here.
	 */
	for (i = 0; i < THREADS_MAX_DISKS; i++)
	{
		sprintf(buf, "%d", i);
		sprintf(name, "DiskDriver%d", i);

		diskInformation[i].mutex = k_semcreate(0); // create a mutex for the disk driver
		DSL_InitList(FALSE,						   // initialize the waiting list for the disk driver
					 i == 0 ? OFFSETOF_DISK_0_NEXT : OFFSETOF_DISK_1_NEXT,
					 &diskInformation[i].waitingProcs,
					 NULL); // TODO: Provide the sort function that sorts by priority

		diskPids[i] = k_spawn(name, DiskDriver, buf, THREADS_MIN_STACK_SIZE * 4, HIGHEST_PRIORITY);
		if (diskPids[i] < 0)
		{
			console_output(TRUE, "start3(): Can't create disk driver %d\n", i);
			stop(1);
		}
	}
	/* Wait for the disk drivers to start. */
	for (i = 0; i < THREADS_MAX_DISKS; i++)
	{
		k_semp(running);
	}

	/*
	 * Create first user-level process and wait for it to finish.
	 */
	sys_spawn("DevicesEntryPoint", DevicesEntryPoint, NULL, 8 * THREADS_MIN_STACK_SIZE, 3);
	sys_wait(&status);

	/*
	 * Terminate the device drivers cleanly
	 */
	return 0;
}

static int ClockDriver(char *arg)
{
	DevicesProcess *pProc = NULL;
	int result;
	int status;

	/*
	 * Let the parent know we are running and enable interrupts.
	 */

	k_semv(running);
	enableInterrupts();

	while (!signaled())
	{
		result = wait_device("clock", &status);
		if (result != 0)
		{
			return 0;
		}

		/*
		 * Compute the current time and wake up any processes
		 * whose time has come.
		 */
	}
	return 0;
}

/**
 *
 */
static int DiskDriver(char *arg)
{
	int result = 0;
	int unit = atoi(arg);
	int currentTrack = 0;
	DeviceControl devRequest;
	DevicesProcess *pRequestingProc = NULL;
	DevicesProcess *pNextRequestingProc = NULL;

	// get disk information
	// set the device name, the driver is called on disk init
	sprintf(diskInformation[unit].deviceName, "disk%d", unit);
	enableInterrupts(); // i/o needs interrupts enabled
	result = GetDiskInfo(unit, &diskInformation[unit], &devRequest);
	if (result != 0)
	{
		console_output(TRUE, "DiskDriver(): Can't get disk information for unit %d\n", unit);
		stop(1);
	}
	else
	{
		console_output(TRUE, "DiskDriver(): %s has %d tracks and %d platters\n",
					   diskInformation[unit].deviceName,
					   diskInformation[unit].tracks,
					   diskInformation[unit].platters);
	}

	k_semv(running); // signal the parent that we are running

	/* Operating loop */
	while (!signaled())
	{

		k_semp(diskInformation[unit].mutex); // wait for a request
	}
	return 0;
}

/*****************************************************************************
   Name - checkKernelMode
   Purpose - Checks the PSR for kernel mode and stops if in user mode
   Parameters -
   Returns -
   Side Effects - Will stop if not in kernel mode
****************************************************************************/
static inline void checkKernelMode(const char *functionName)
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
 * @brief Initializes the system call handlers.
 *
 * This function initializes the system call vector with the appropriate system call handlers
 * for this version of the kernel. It sets the dispatcher for the following system calls:
 *   - SYS_SLEEP
 *   - SYS_DISKREAD
 *   - SYS_DISKWRITE
 *   - SYS_DISKINFO
 *
 * @return void
 */
static void InitializeHandlers(void)
{

	for (int i = SUPPORTED_SYS_CALL_START; i <= SUPPORTED_SYS_CALL_END; i++)
	{
		systemCallVector[i] = sys_call_dispatcher;
	}
}

/**
 * @brief System call dispatcher.
 * This function is called by the system call interrupt handler to dispatch
 * the appropriate system call handler for this version of the kernel.
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
	case SYS_SLEEP:
		/*TODO: */
		/* sleep for a number of seconds */
		console_output(FALSE, "Sleeping for %d seconds\n", (int)args->arguments[0]);
		break;

	case SYS_DISKREAD:
		/*TODO: */
		/* read from a disk */
		console_output(FALSE, "Reading from disk %d\n", (int)args->arguments[0]);
		break;

	case SYS_DISKWRITE:
		/*TODO: */
		/* write to a disk */
		console_output(FALSE, "Writing to disk %d\n", (int)args->arguments[0]);
		break;

	case SYS_DISKINFO:
		/*TODO: */
		/* get the size of a disk */
		console_output(FALSE, "Getting size of disk %d\n", (int)args->arguments[0]);
		break;
	default:
		/* Do nothing as we don't want to overwrite previously declared handlers */
		break;
	}
	/* set mode to user mode before returning.*/
	setUserMode();
	/* --------------------------- USER-SPACE --------------------------- */
}

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

/*
 * Enables the interrupts.
 */
static inline void enableInterrupts()
{
	set_psr(get_psr() | PSR_INTERRUPTS);
}

// DISC CONTROLLER CODE

/**
 * The DiskInfo function retrieves the disk information for a given disk unit.
 * It uses the DISK_INFO command to get the disk information and fills the provided
 */
static int GetDiskInfo(int unit, DiskInformation *diskInfo, DeviceControl *devRequest)
{
	/* check for kernel mode */
	checkKernelMode(__func__);
	union DiskInfoResult diskResult = {0}; /* disk information result */
	// uint32_t diskResult = 0; /* disk result */

	/* check for valid arguments */
	if (diskInfo == NULL || devRequest == NULL || unit < 0 || unit >= THREADS_MAX_DISKS)
	{
		return -1; /* invalid arguments */
	}

	/* set the command to DISK_INFO and call the device driver */
	devRequest->command = DISK_INFO;

	/* using device control and the control block, send the request to read*/
	device_control(diskInfo->deviceName, *devRequest);

	/* wait for the result */
	wait_device(diskInfo->deviceName, &diskResult.rawResult);

	if (diskResult.rawResult < 0)
	{
		return -1; /* error getting disk information */
	}
	else
	{
		// print result
		console_output(FALSE, "RESULT: %d\n", diskResult.rawResult);
		console_output(FALSE, "TRACKS: %d\n", diskResult.info.trackCount);
		console_output(FALSE, "PLATTERS: %d\n", diskResult.info.platterCount);
		console_output(FALSE, "RESULT CODE: %d\n", diskResult.info.resultCode);
	}

	/* set the disk information structure */
	diskInfo->tracks = diskResult.info.trackCount;		   /* number of tracks on the disk */
	diskInfo->platters = diskResult.info.platterCount + 1; /* number of platters on the disk */

	// console_output(FALSE, "DiskInfo(): Disk %d has %d tracks and %d platters\n", unit, diskInfo->tracks, diskInfo->platters);

	return 0;
}
