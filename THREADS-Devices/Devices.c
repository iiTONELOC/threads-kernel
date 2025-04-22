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
#include <DeviceUtils.h>

static int running;															   /*semaphore to synchronize drivers and start3*/
interrupt_handler_t *handlers;												   /*From THREADS*/
static DSL_List sleeperList = {0};											   /* list of sleeping processes */
static IO_Request pendingIoRequestTable[MAXPROC] = {0};						   /* pending I/O request table */
DiskInformation diskInformation[THREADS_MAX_DISKS] = {0};					   /* disk information structure */
static DevicesProcess devicesProcessTable[MAXPROC] = {0};					   /* device process table (Static storage) */
void (*systemCallVector[THREADS_MAX_SYSCALLS])(system_call_arguments_t *args); /* system call array of function pointers */

static void setUserMode(void);
static int DiskDriver(char *);
static int ClockDriver(char *);
static void setKernelMode(void);
static void ManageSleepers(void);
static void InitializeHandlers(void);
extern int DevicesEntryPoint(char *);
static inline void enableInterrupts();
static void sys_sleep(system_call_arguments_t *args);
static void sys_diskInfo(system_call_arguments_t *args);
static inline void checkKernelMode(const char *functionName);
static void sys_call_dispatcher(system_call_arguments_t *args);
static int DiskSeek(int unit, int track, int sector, int platter);
static void sys_io(system_call_arguments_t *args, enum TDISK_MODE mode);
static int GetDiskInfoOnInit(int unit, DiskInformation *diskInfo, DeviceControlBlock *devRequest);

int SystemCallsEntryPoint(char *arg)
{
	int i;
	int status;
	char buf[25];
	char name[128];
	int clockPID = 0;

	checkKernelMode(__func__);

	/* Assign system call handlers */
	InitializeHandlers();

	/* Init the tables */
	InitializeTables(devicesProcessTable, pendingIoRequestTable);

	/* Init the sleeper list */
	DSL_InitList(FALSE, OFFSETOF_SLEEP_NEXT, &sleeperList, compareSleepRequest);

	/*
	 * Create clock device driver
	 * Use a semaphore or mailbox for coordinating the start of the drive
	 */
	running = k_semcreate(0);
	clockPID = k_spawn("Clock driver", ClockDriver, NULL, THREADS_MIN_STACK_SIZE, HIGHEST_PRIORITY);
	if (clockPID < 0)
	{
		console_output(FALSE, "start3(): Can't create clock driver\n");
		stop(1);
	}

	/*
	 * Wait for the clock driver to start
	 */
	k_semp(running);

	/*
	 * Create the disk device drivers here.
	 */
	for (i = 0; i < THREADS_MAX_DISKS; i++)
	{
		sprintf(buf, "%d", i);
		sprintf(name, "DiskDriver%d", i);

		/* Initialize the disc's request list - sorted by startTrack if not FCFS */
		DSL_InitList(FALSE,
					 i == 0 ? OFFSETOF_DISK_0_NEXT : OFFSETOF_DISK_1_NEXT,
					 &diskInformation[i].requestList,
					 TDISK_ALGO == TDISK_FCFS ? NULL : compareIoRequest);
		diskInformation[i].index = i;							   // set the index of the disk driver
		diskInformation[i].mutex = mailbox_create(1, sizeof(int)); // Create a mutex for the process
		diskInformation[i].pid = k_spawn(name, DiskDriver, buf, THREADS_MIN_STACK_SIZE * 4, HIGHEST_PRIORITY);
		if (diskInformation[i].pid < 0)
		{
			console_output(FALSE, "start3(): Can't create disk driver %d\n", i);
			stop(1);
		}
	}

	/* Wait for the disk drivers to start. */
	for (i = 0; i < THREADS_MAX_DISKS; i++)
	{
		k_semp(running);
	}

	/*
	 * Create first user-level process (but in kernel mode) and wait for it to finish.
	 */
	sys_spawn("DevicesEntryPoint", DevicesEntryPoint, NULL, 8 * THREADS_MIN_STACK_SIZE, 3);
	sys_wait(&status);

	/*
	 * Terminate the device drivers cleanly
	 */

	// kill the clock driver
	k_kill(clockPID, SIG_TERM);
	k_wait(&status);

	// kill the disk drivers
	for (i = 0; i < THREADS_MAX_DISKS; i++)
	{
		k_kill(diskInformation[i].pid, SIG_TERM);
		mailbox_send(diskInformation[i].mutex, NULL, 0, TRUE); // unblock the process
		k_wait(&status);
	}

	// free the semaphore
	k_semfree(running);
	running = -1;

	return 0;
}

/**
 * @brief Clock driver
 *
 * This function is the main entry point for the clock driver.
 * It initializes the clock information, sets up the device name,
 * and enters the operating loop to process clock requests.
 */
static int ClockDriver(char *arg)
{
	int result;
	int status;

	/* Let the parent know we are running and enable interrupts. */
	k_semv(running);
	enableInterrupts();

	while (!signaled())
	{
		result = wait_device("clock", &status);
		if (result != 0)
		{
			return 0;
		}

		disableInterrupts();
		ManageSleepers();
		enableInterrupts();
	}

	return 0;
}

/**
 * @brief Disk driver
 *
 * This function is the main entry point for the disk driver.
 * It initializes the disk information, sets up the device name,
 * and enters the operating loop to process disk requests.
 */
static int DiskDriver(char *arg)
{
	int result = 0;										// disk info result
	int status = 0;										// disk io result
	int unit = atoi(arg);								// disk unit number
	char *readBuffer = NULL;							// io read buffer
	char *writeBuffer = NULL;							// io write buffer
	bool needToMoveArm = FALSE;							// need to move the arm to the correct track
	DeviceControlBlock devRequest;						// device control block structure
	bool opWillExceedMaxSectors = FALSE;				// operation will exceed max sectors
	DevicesProcess *pRequestingProc = NULL;				// requesting process
	DevicesProcess *pNextRequestingProc = NULL;			// next requesting process
	DiskInformation *diskInfo = &diskInformation[unit]; // get the disk information for this unit

	// set the device name
	sprintf(diskInfo->deviceName, "disk%d", unit);
	enableInterrupts(); // i/o needs interrupts enabled
	result = GetDiskInfoOnInit(unit, diskInfo, &devRequest);
	if (result != 0)
	{
		console_output(FALSE, "DiskDriver(): Can't get disk information for unit %d\n", unit);
		stop(1);
	}

	k_semv(running); // signal the parent that we are running

	/* Main Operating loop */
	while (!signaled())
	{
		mailbox_receive(diskInfo->mutex, NULL, 0, TRUE); // block until a request is received
		/* After we have been awoken. Check for requests */
		disableInterrupts();
		if (diskInfo->requestList.length > 0)
		{

// TODO: Implement a get next request function for all four algos
#if TDISK_ALGO == TDISK_FCFS

			IO_Request *pRequest = (IO_Request *)DSL_Pop(&diskInfo->requestList);
#endif

			// operation loop - we can only process one sector at a time
			while (pRequest->numSectorsCompleted < pRequest->numSectors)
			{
				needToMoveArm = diskInfo->currentTrack != pRequest->startTrack;
				opWillExceedMaxSectors = (pRequest->numSectors + pRequest->startSector) >= THREADS_DISK_SECTOR_COUNT ? TRUE : FALSE;

				/* START VARIABLE LOGIC DEPENDING ON ALGORITHM USED */
				/*-------------------------------------------------*/
				// TODO: Implement the disk seek function for all four algos
				// Currently FCFS
				if (needToMoveArm)
				{
					// if the disk seek fails, we need to remove the request from the list and unblock the process ??
					SEEK_HANDLE_ERRS(diskInfo, pRequest, status);
				}

				// TODO: Handle ops spanning multiple tracks for all four algos
				if (opWillExceedMaxSectors && diskInfo->currentSector == 0)
				{
					pRequest->startTrack = ((pRequest->startTrack + 1) % diskInfo->tracks);
					SEEK_HANDLE_ERRS(diskInfo, pRequest, status);
				}

				/* END VARIABLE LOGIC DEPENDING ON ALGORITHM USED */
				/*-----------------------------------------------*/

				/* This Logic here should not change, regardless of algo approach, this reads/writes the actual sectors to/from disk*/
				if (pRequest->mode == TDISK_READ)
				{
					readBuffer = pRequest->readBuffer + (pRequest->numSectorsCompleted * THREADS_DISK_SECTOR_SIZE);
				}
				else
				{
					writeBuffer = pRequest->writeBuffer + (pRequest->numSectorsCompleted * THREADS_DISK_SECTOR_SIZE);
				}
				enableInterrupts(); // enable interrupts before doing the I/O operation
				// write or read our data to/from the disk
				device_control(diskInfo->deviceName, (DeviceControlBlock){
														 pRequest->mode,
														 pRequest->startPlatter,
														 pRequest->startSector,
														 readBuffer,
														 writeBuffer,
														 THREADS_DISK_SECTOR_SIZE});

				// wait for the result
				union DiskIoResult ioResult = {0}; /* IO request result */
				wait_device(diskInfo->deviceName, &ioResult.rawResult);
				disableInterrupts();
				if (ioResult.info.resultCode < 0)
				{
					console_output(FALSE, "DiskDriver()::HandleFirstComeFirstServed: Error processing request for process %d\n", pRequest->forPid);
					pRequest->opResultStatus = -1; // set the operation status to error
					break;
				}
				else
				{

					pRequest->numSectorsCompleted++;
					pRequest->startSector = (pRequest->startSector + 1) % THREADS_DISK_SECTOR_COUNT;
					diskInfo->currentSector = pRequest->startSector;
				}
			} // end of operation loop
			pRequest->opResultStatus = 0;						  // set the operation status to success
			pRequest->numSectorsCompleted = pRequest->numSectors; // set the number of sectors completed to the total number of sectors

			enableInterrupts();
			// wake up requesting process
			mailbox_send(devicesProcessTable[pRequest->forPid % MAXPROC].mutex, NULL, 0, TRUE);
		}
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
 * @brief Manages the list of sleeping processes.
 *
 * Checks the list of sleeping processes and wakes up any processes
 * that have reached their wake time by unblocking them. Unblocked
 * processes are removed from the sleeper list.
 */
static void ManageSleepers(void)
{
	DevicesProcess *pProc = NULL;

	if (sleeperList.length > 0)
	{
		// the list is sorted by sleep time
		pProc = ((DevicesProcess *)sleeperList.pHead);

		int currentProcMutex = pProc->mutex;

		// check for processes that slept long enough
		while (pProc != NULL && pProc->wakeTime <= system_clock())
		{
			pProc->wakeTime = 0; // reset the wake time

			// move to the next process - not the mutex yet, we still need it
			pProc = (DevicesProcess *)pProc->pNext;

			// remove the process to wake
			DSL_Pop(&sleeperList);

			// unblock the process
			enableInterrupts();
			mailbox_send(currentProcMutex, NULL, 0, TRUE);
			disableInterrupts();

			if (pProc != NULL)
				// get the next process mutex
				currentProcMutex = pProc->mutex;
		}
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
		return;
	}

	/* ensure we are in kernel mode */
	setKernelMode();

	// Call the appropriate system call handler
	switch (args->call_id)
	{
	case SYS_SLEEP:
		sys_sleep(args);
		break;

	case SYS_DISKREAD:
		sys_io(args, TDISK_READ);
		break;

	case SYS_DISKWRITE:
		sys_io(args, TDISK_WRITE);
		break;

	case SYS_DISKINFO:
		/* get disk information */
		sys_diskInfo(args);
		break;
	default:
		/* Do nothing as we don't want to overwrite previously declared handlers */
		break;
	}
	/* set mode to user mode before returning.*/
	setUserMode();
	/* --------------------------- USER-SPACE --------------------------- */
}

static void sys_sleep(system_call_arguments_t *args)
{
	/* check for kernel mode */
	checkKernelMode(__func__);

	/* check for valid arguments */
	if (args == NULL || args->arguments[0] < 0)
	{
		args->arguments[3] = -1;
		return;
	}

	/* disable interrupts */
	disableInterrupts();

	/* get the current process */
	int pid = k_getpid();
	DevicesProcess *pProc = &devicesProcessTable[pid % MAXPROC];
	pProc->pid = pid;

	/* Figure wakeup time*/
	pProc->wakeTime = (size_t)(system_clock() + ((size_t)((int)args->arguments[0] * SECONDS_IN_MILLISECOND * NUM_MILLISEC_IN_MICROSEC)));

	/* insert the process into the sleeper list */
	DSL_InsertNode((void *)pProc, &sleeperList);

	enableInterrupts();

	/* block the process */
	mailbox_receive(pProc->mutex, NULL, 0, TRUE);

	args->arguments[3] = 0; /* success */
	return;
}

/**
 * @brief Sets the processor mode to user mode.
 */
static void setUserMode(void)
{
	set_psr(get_psr() & ~PSR_KERNEL_MODE);
}

/**
 * @brief Sets the processor mode to kernel mode.
 */
static void setKernelMode(void)
{
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

static void sys_diskInfo(system_call_arguments_t *args)
{
	/* check for kernel mode */
	int unit = -1;
	checkKernelMode(__func__);

	if ((unit = GetUnitFromArgs(args)) < 0)
	{
		console_output(FALSE, "DiskInfo(): Invalid arguments: Disk not specified.\n");
		args->arguments[3] = -1; /* invalid arguments */
		return;
	}

	disableInterrupts();
	DiskInformation *diskInfo = &diskInformation[unit];
	DeviceControlBlock devRequest = {0};	 /* device control block */
	args->arguments[2] = diskInfo->tracks;	 /* number of tracks on the disk */
	args->arguments[4] = diskInfo->platters; /* number of platters on the disk */

	enableInterrupts();

	/* set the remaining return values */
	args->arguments[3] = 0;
	args->arguments[0] = THREADS_DISK_SECTOR_SIZE;
	args->arguments[1] = THREADS_DISK_SECTOR_COUNT;
}

/**
 * * @brief System call handler for reading and writing to disks.
 */
static void sys_io(system_call_arguments_t *args, enum TDISK_MODE mode)
{
	/* check for kernel mode */
	int unit = -1;
	checkKernelMode(__func__);

	if ((unit = GetUnitFromArgs(args)) < 0)
	{
		console_output(FALSE, "DiskInfo(): Invalid arguments: Disk not specified.\n");
		args->arguments[3] = -1; /* invalid arguments */
		return;
	}

	bool isPlatterValid = args->arguments[2] >= 0 && args->arguments[2] < diskInformation[unit].platters;
	bool isTrackValid = args->arguments[3] >= 0 && args->arguments[3] < diskInformation[unit].tracks;
	bool isSectorValid = args->arguments[4] >= 0 && args->arguments[4] < THREADS_DISK_SECTOR_COUNT;
	bool isSectorsValid = args->arguments[5] > 0 && args->arguments[5] <= THREADS_DISK_SECTOR_COUNT;

	/* check for valid arguments */
	if (args->arguments[1] == NULL || !isPlatterValid || !isTrackValid || !isSectorValid || !isSectorsValid)
	{
		args->arguments[3] = -1; /* invalid arguments */
		return;
	}

	int pid = k_getpid();
	int safeId = pid % MAXPROC; /* safe id for the process */
	disableInterrupts();
	/* handle io operations by constructing an IO_Request for the current process */
	DevicesProcess *pCurrentProcess = &devicesProcessTable[safeId];

	/* ensure the current process doesn't have an active request waiting */
	if (pCurrentProcess->ioRequest != NULL)
	{
		console_output(FALSE, "DiskInfo(): Process %d already has an active request.\n", pCurrentProcess->pid);
		args->arguments[3] = -1; /* invalid arguments */
		enableInterrupts();
		return;
	}

	/* initialize the IO_Request structure for this process */
	pCurrentProcess->ioRequest = &pendingIoRequestTable[safeId];
	SetIoRequest(pCurrentProcess->ioRequest, &diskInformation[unit].deviceName, pid, args, mode);

	/*Add the process to the queue for the correct driver  */
	DSL_InsertNode((void *)pCurrentProcess->ioRequest, &diskInformation[unit].requestList);

	/* set the wake time to 0, as we are not sleeping */
	pCurrentProcess->wakeTime = 0;

	/* enable interrupts and block ourselves */
	enableInterrupts();
	/* signal the disk to wake up */
	mailbox_send(diskInformation[unit].mutex, NULL, 0, TRUE);
	/* wait for the disk driver to finish the request */
	mailbox_receive(pCurrentProcess->mutex, NULL, 0, TRUE);
	/* disable interrupts again */
	disableInterrupts();
	/* check for errors */
	if (pCurrentProcess->ioRequest->opResultStatus < 0)
	{
		console_output(FALSE, "DiskInfo(): Error in disk operation.\n");
		args->arguments[3] = -1; /* invalid arguments */
	}
	else
	{
		args->arguments[3] = 0; /* success */
	}
	args->arguments[0] = pCurrentProcess->ioRequest->opResultStatus; /* operation status */
	pCurrentProcess->ioRequest = NULL;								 /* reset the request */
	enableInterrupts();												 /* enable interrupts again */
}

/**
 * The DiskInfo function retrieves the disk information for a given disk unit.
 * It uses the DISK_INFO command to get the disk information and fills the provided
 */
static int GetDiskInfoOnInit(int unit, DiskInformation *diskInfo, DeviceControlBlock *devRequest)
{
	/* check for kernel mode */
	checkKernelMode(__func__);
	union DiskInfoResult diskResult = {0}; /* disk information result */

	/* check for valid arguments */
	if (diskInfo == NULL || devRequest == NULL || unit < 0 || unit >= THREADS_MAX_DISKS)
	{
		return -1; /* invalid arguments */
	}

	/* set the command to DISK_INFO and call the device driver */
	devRequest->command = DISK_INFO;

	/* using device control and the control block, send the request to DISK_INFO*/
	device_control(diskInfo->deviceName, *devRequest);

	/* wait for the result */
	wait_device(diskInfo->deviceName, &diskResult.rawResult);

	if (diskResult.rawResult < 0 || (DiskSeek(unit, 0, 0, 0) != 0))
	{
		console_output(FALSE, "DiskInfo(): Error getting disk information for unit %d\n", unit);
		return -1; /* error getting disk information */
	}

	disableInterrupts();
	/* set the disk information structure */
	diskInfo->currentTrack = 0;						   /* current track being processed */
	diskInfo->currentSector = 0;					   /* current sector being processed */
	diskInfo->currentPlatter = 0;					   /* current platter being processed */
	diskInfo->currentRequest = NULL;				   /* current request being processed */
	diskInfo->tracks = diskResult.info.trackCount;	   /* number of tracks on the disk */
	diskInfo->platters = diskResult.info.platterCount; /* number of platters on the disk */

	enableInterrupts();

	return 0;
}

/**
 * @brief Move the arm to a new track.
 */
static int DiskSeek(int unit, int track, int sector, int platter)
{
	/* check for kernel mode */
	checkKernelMode(__func__);

	/* check for valid arguments */
	if (unit < 0 || unit >= THREADS_MAX_DISKS || track < 0 || sector < 0 || platter < 0)
	{
		return -1; /* invalid arguments */
	}

	/* set the command to DISK_SEEK and call the device driver */
	device_control(diskInformation[unit].deviceName, (DeviceControlBlock){DISK_SEEK, track, sector, NULL, NULL, platter});

	/* wait for the result */
	union DiskIoResult ioResult = {0}; /* IO request result */
	wait_device(diskInformation[unit].deviceName, &ioResult.rawResult);
	if (ioResult.info.resultCode < 0)
	{
		console_output(FALSE, "DiskSeek(): Error seeking disk %d to track %d, sector %d, platter %d\n", unit, track, sector, platter);
	}

	diskInformation[unit].currentTrack = track;
	diskInformation[unit].currentSector = sector;
	diskInformation[unit].currentPlatter = platter;

	return ioResult.info.resultCode;
}
