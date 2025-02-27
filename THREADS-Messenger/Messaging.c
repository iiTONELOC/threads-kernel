/* ------------------------------------------------------------------------
   Messaging.c
   College of Applied Science and Technology
   The University of Arizona
   CYBV 489

   Student Names: Anthony Tropeano, Connor Stackhouse

   ------------------------------------------------------------------------ */
#include <DoublyLinkedList.h>
#include <MailBox.h>
#include <MailUtils.h>
#include <Messaging.h>
#include <MessagingProcess.h>
#include <Messenger.h>
#include <Scheduler.h>
#include <stdint.h>
#include <string.h>
#include <THREADSLib.h>
#include <Windows.h>

   /* ------------------------- Prototypes ----------------------------------- */
static void nullsys(system_call_arguments_t* args);

typedef void (*interrupt_handler_t)(char deviceId[32], uint8_t command, uint32_t status);

static int check_io_messaging(void);
static void InitializeHandlers(void);
extern int MessagingEntryPoint(void*);
MessagingProcess* runningMessengerProcess = NULL;
static void checkKernelMode(const char* functionName);
void IOInterruptHandler(char deviceId[32], uint8_t command, uint32_t status);
void TimerInterruptHandler(char deviceId[32], uint8_t command, uint32_t status);
void SysCallInterruptHandler(char deviceId[32], uint8_t command, uint32_t status);

struct psr_bits
{
	unsigned int cur_int_enable : 1;
	unsigned int cur_mode : 1;
	unsigned int prev_int_enable : 1;
	unsigned int prev_mode : 1;
	unsigned int unused : 28;
};

union psr_values
{
	struct psr_bits bits;
	unsigned int integer_part;
};

/* -------------------------- Globals ------------------------------------- */

/* Obtained from THREADS*/
interrupt_handler_t* handlers;

/* system call array of function pointers */
void (*systemCallVector[THREADS_MAX_SYSCALLS])(system_call_arguments_t* args);

static int waitingOnDevice = 0;
static DeviceManagementData devices[THREADS_MAX_DEVICES];

/* ------------------------------------------------------------------------
	 Name - SchedulerEntryPoint
	 Purpose - Initializes mailboxes and interrupt vector.
			   Start the Messaging test process.
	 Parameters - one, default arg passed by k_spawn that is not used here.
----------------------------------------------------------------------- */
int SchedulerEntryPoint(void* arg)
{
	int result = 0;
	// check for kernel mode
	checkKernelMode(__func__);

	/* Disable interrupts */
	disableInterrupts();

	/* set this to the real check_io function. */
	check_io = check_io_messaging;

	/* Initialize the mail box table, slots, & other data structures.
	 * Initialize int_vec and sys_vec, allocate mailboxes for interrupt
	 * handlers.  Etc... */
	InitMessagingTables();

	/* Initialize the devices and their mailboxes. */
	/* Allocate mailboxes for use by the interrupt handlers */
	InitDeviceMailBoxes(devices);

	InitializeHandlers();

	enableInterrupts();

	/* Create a process for Messaging, then block on a wait until messaging exits.*/
	result = k_spawn("MessagingEntryPoint", MessagingEntryPoint,
		NULL, 4 * THREADS_MIN_STACK_SIZE, HIGHEST_PRIORITY);

	if (result < 0)
	{
		console_output(FALSE,
			"SchedulerEntryPoint(): spawn for MessagingEntryPoint returned an error (%d), stopping...\n", result);
		stop(1);
	}

	/* wait for the MessagingEntryPoint to finish */
	k_wait(&result);

	k_exit(result);

	return 0;
} /* SchedulerEntryPoint */

/* ------------------------------------------------------------------------
   Name - mailbox_create
   Purpose - gets a free mailbox from the table of mailboxes and initializes it
   Parameters - maximum number of slots in the mailbox and the max size of a msg
				sent to the mailbox.
   Returns - -1 to indicate that no mailbox was created, or a value >= 0 as the
			 mailbox id.
   ----------------------------------------------------------------------- */
int mailbox_create(int slots, int slot_size)
{
	checkKernelMode(__func__);
	int result = -1;

	result = ReuseMailbox(GetNextEmptyMailbox(), slots, slot_size);

	return result;
} /* mailbox_create */

/* ------------------------------------------------------------------------
   Name - mailbox_send
   Purpose - Put a message into a slot for the indicated mailbox.
			 Block the sending process if no slot available.
   Parameters - mailbox id, pointer to data of msg, # of bytes in msg.
   Returns - zero if successful, -1 if invalid args.
   Side Effects - none.
   ----------------------------------------------------------------------- */
int mailbox_send(int mboxId, void* pMsg, int msg_size, int wait)
{
	checkKernelMode(__func__);
	enableInterrupts();
	/* Producer */
	int result = -1;
	MailBox* pMailBox;
	int myPid = k_getpid();
	MessagingProcess* pProcess;

	/* Validate the input*/
	if (mboxId < 0 || !pMsg || msg_size < 0 || myPid < 0)
	{
		/* Invalid args, return -1 */
		return result;
	}

	disableInterrupts();
	/*Get the mailbox*/
	if (!(pMailBox = &MAIL_BOXES[GetMailboxIdx(mboxId)]))
	{
		enableInterrupts();
		return result;
	}

	/* Get the current process in the messaging process table */
	pProcess = FindProcessInTable(myPid, TRUE);

	/* If the mailbox has zero slots*/
	if (pMailBox->slotCount == 0)
	{
		/* Handle the case where the mailbox has zero slots*/
		result = HandleSendMailZeroSlots(pMailBox, pProcess, pMsg, msg_size, wait);
	}
	else
	{
		/* Handle the case where the mailbox has slots*/
		result = HandleSendMailWithSlots(pMailBox, pProcess, pMsg, msg_size, wait);
	}

	disableInterrupts();
	/* Unblock any processes waiting to receive a message */
	if (pMailBox->waitingProcsRecvList.count > 0)
	{
		UnblockMessagingProcess(((MessagingProcess*)Pop(&pMailBox->waitingProcsRecvList))->pid, MP_READY);
	}
	enableInterrupts();

	return result;
}

/* ------------------------------------------------------------------------
   Name - mailbox_receive
   Purpose - Put a message into a slot for the indicated mailbox.
			 Block the sending process if no slot available.
   Parameters - mailbox id, pointer to data of msg, # of bytes in msg.
   Returns - zero if successful, -1 if invalid args.
   Side Effects - none.
   ----------------------------------------------------------------------- */
int mailbox_receive(int mboxId, void* pMsg, int msg_size, int wait)
{
	checkKernelMode(__func__);
	enableInterrupts();
	/*Consumer*/
	int result = -1;
	MailBox* pMailBox;
	int myPid = k_getpid();
	MessagingProcess* pProcess;

	/* Validate the input */
	if (mboxId < 0 || !pMsg || msg_size < 0 || myPid < 0)
	{
		/* Invalid args, return -1 */
		return result;
	}

	disableInterrupts();
	/*Get the mailbox*/
	if (!(pMailBox = &MAIL_BOXES[GetMailboxIdx(mboxId)]))
	{
		enableInterrupts();
		return result;
	}

	/* Get the current process in the messaging process table */
	pProcess = FindProcessInTable(myPid, TRUE);
	// enableInterrupts();

	if (pMailBox->slotCount == 0)
	{

		/* Handle the case where the mailbox has zero slots*/
		result = HandleReceiveMailZeroSlots(pMailBox, pProcess, pMsg, msg_size, wait);
	}
	else
	{
		/* Handle the case where the mailbox has slots*/
		result = HandleReceiveMailWithSlots(pMailBox, pProcess, pMsg, msg_size, wait);
	}

	if (pMailBox->waitingProcsSendList.count > 0)
	{
		UnblockMessagingProcess(((MessagingProcess*)Pop(&pMailBox->waitingProcsSendList))->pid, MP_READY);
	}

	return result;
}

/* ------------------------------------------------------------------------
   Name - MboxRelease
   ----------------------------------------------------------------------- */
int mailbox_free(int mboxId)
{
	checkKernelMode(__func__);
	int result = -1;

	return result;
}

int wait_device(char* deviceName, int* status)
{

	int result = 0;
	uint32_t deviceHandle = -1;
	checkKernelMode("waitdevice");

	enableInterrupts();

	if (strcmp(deviceName, "clock") == 0)
	{
		deviceHandle = THREADS_CLOCK_DEVICE_ID;
	}
	else
	{
		deviceHandle = device_handle(deviceName);
	}

	if (deviceHandle >= 0 && deviceHandle < THREADS_MAX_DEVICES)
	{
		/* set a flag that there is a process waiting on a device. */
		waitingOnDevice++;
		mailbox_receive(devices[deviceHandle].deviceMbox, status, sizeof(int), TRUE);
		waitingOnDevice--;
	}
	else
	{
		console_output(FALSE, "Unknown device type.");
		stop(-1);
	}

	/* spec says return -1 if zapped. */
	if (signaled())
	{
		result = -5;
	}

	return result;
}

int check_io_messaging(void)
{
	checkKernelMode(__func__);
	if (waitingOnDevice)
	{
		return 1;
	}
	return 0;
}

/* an error method to handle invalid syscalls */
static void nullsys(system_call_arguments_t* args)
{
	console_output(FALSE, "nullsys(): Invalid syscall %d. Halting...\n", args->call_id);
	stop(1);
} /* nullsys */

/*****************************************************************************
   Name - checkKernelMode
   Purpose - Checks the PSR for kernel mode and halts if in user mode
   Parameters -
   Returns -
****************************************************************************/
static inline void checkKernelMode(const char* functionName)
{
	union psr_values psrValue;

	psrValue.integer_part = get_psr();
	if (psrValue.bits.cur_mode == 0)
	{
		console_output(FALSE, "Kernel mode expected, but function called in user mode.\n");
		stop(1);
	}
}

/*****************************************************************************
   Name - InitializeHandlers
   Purpose - Initializes the interrupt handlers
   Parameters -
   Returns -
****************************************************************************/

static void InitializeHandlers(void)
{
	handlers = get_interrupt_handlers();
	// handlers[THREADS_TIMER_INTERRUPT] = TimerInterruptHandler;
	// handlers[THREADS_IO_INTERRUPT] = IOInterruptHandler;
	// handlers[THREADS_SYS_CALL_INTERRUPT] = SysCallInterruptHandler;

	/* init the system call vector */
	for (int i = 0; i < THREADS_MAX_SYSCALLS; i++)
	{
		systemCallVector[i] = nullsys;
	}
}

/*****************************************************************************
   Name - IOInterruptHandler
   Purpose - Handles IO interrupts
   Parameters -
   Returns -
****************************************************************************/

void IOInterruptHandler(char deviceId[32], uint8_t command, uint32_t status)
{
	return;
}

/*****************************************************************************
   Name - TimerInterruptHandler
   Purpose - Handles Timer interrupts
   Parameters -
   Returns -
****************************************************************************/
void TimerInterruptHandler(char deviceId[32], uint8_t command, uint32_t status)
{
	/* Copied from Scheduler - Not sure what to do here yet */
	console_output(FALSE, "Timer Interrupt, pid: %d\n", k_getpid());

	int pid = k_getpid();

	if (!runningMessengerProcess && pid > 0)
	{
		runningMessengerProcess = FindProcessInTable(pid, TRUE);
	}

	static int elapsed = 0;			  // time elapsed since last context switch
	static int lastTime = 0;		  // last time the clock interrupt was called
	int currentTime = system_clock(); // current time in μs but

	// if there is a running process and it doesn't have a start time, set it
	if (runningMessengerProcess != NULL && runningMessengerProcess->startTime == 0)
	{
		// needs to be in μs
		runningMessengerProcess->startTime = currentTime;
	}

	// calculate the time elapsed since the last context switch
	if (lastTime != 0)
	{
		// should be in μs
		elapsed += (currentTime - lastTime);
	}

	// update the last time the clock interrupt was called
	lastTime = currentTime;

	// if there is a running process, update its elapsed and cpu time
	if (runningMessengerProcess != NULL)
	{
		runningMessengerProcess->elapsedTime += elapsed;
		runningMessengerProcess->cpuTime += (elapsed / 1000);
	}

	// check if the elapsed time is greater than the quantum for the running process
	if (runningMessengerProcess != NULL && runningMessengerProcess->elapsedTime >= runningMessengerProcess->quantum)
	{
		elapsed = 0;
		runningMessengerProcess->elapsedTime = 0;

		time_slice();
	}
	else
	{
		elapsed = 0;
	}
}

void SysCallInterruptHandler(char deviceId[32], uint8_t command, uint32_t status)
{
	return;
}