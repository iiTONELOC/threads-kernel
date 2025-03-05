/* ------------------------------------------------------------------------
   Messaging.c
   College of Applied Science and Technology
   The University of Arizona
   CYBV 489

   Student Names: Anthony Tropeano, Connor Stackhouse

   ------------------------------------------------------------------------ */
#include <THREADSLib.h>
#include <DoublyLinkedList.h>
#include <MailBox.h>
#include <MailUtils.h>
#include <Messaging.h>
#include <MessagingProcess.h>
#include <Messenger.h>
#include <Scheduler.h>
#include <stdint.h>
#include <string.h>
#include <Windows.h>
#include "MailSlot.h"

/* ------------------------- Prototypes ----------------------------------- */
static int check_io_messaging(void);
static void InitializeHandlers(void);
extern int MessagingEntryPoint(void *);
static void nullsys(system_call_arguments_t *args);
static void checkKernelMode(const char *functionName);
static void IOInterruptHandler(char deviceId[32], uint8_t command, uint32_t status);
static void TimerInterruptHandler(char deviceId[32], uint8_t command, uint32_t status);
typedef void (*interrupt_handler_t)(char deviceId[32], uint8_t command, uint32_t status);
static void SystemCallInterruptHandler(char deviceId[32], uint8_t command, uint32_t status);

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
interrupt_handler_t *handlers;
static int waitingOnDevice = 0;
MessagingProcess *runningMessengerProcess = NULL;
static DeviceManagementData devices[THREADS_MAX_DEVICES];
/* system call array of function pointers */
void (*systemCallVector[THREADS_MAX_SYSCALLS])(system_call_arguments_t *args);

/* ------------------------------------------------------------------------
	 Name - SchedulerEntryPoint
	 Purpose - Initializes mailboxes and interrupt vector.
			   Start the Messaging test process.
	 Parameters - one, default arg passed by k_spawn that is not used here.
----------------------------------------------------------------------- */
int SchedulerEntryPoint(void *arg)
{
	int result = 0;
	// check for kernel mode
	checkKernelMode(__func__);
	/* Disable interrupts */
	disableInterrupts();

	/* set this to the real check_io function. */
	check_io = check_io_messaging;

	InitMessagingTables();
	InitializeDevices(devices);
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
	disableInterrupts();
	int result = -1;

	result = ReuseMailbox(GetNextEmptyMailbox(), slots, slot_size);
	enableInterrupts();
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
int mailbox_send(int mboxId, void *pMsg, int msg_size, int wait)
{
	checkKernelMode(__func__);

	/* Producer */
	int result = -1;
	MailBox *pMailBox;
	int myPid = k_getpid();
	MailSlot *pMailSlot = NULL;
	MessagingProcess *pWaitingToRecvProcess;

	/* Validate the input*/
	if (mboxId < 0 || msg_size < 0 || myPid < 0)
	{
		/* Invalid args, return -1 */
		return result;
	}

	disableInterrupts();
	runningMessengerProcess = FindProcessInTable(k_getpid());
	/* Ensure the mailbox exists */
	if (((pMailBox = &MAIL_BOXES[GetMailboxIdx(mboxId)]) == NULL) ||
		pMailBox->status == MB_STATUS_EMPTY ||
		pMailBox->status == MB_STATUS_RELEASED)
	{
		enableInterrupts();
		return -1;
	}

	/* Grab a slot from the slot table*/
	if ((pMailSlot = GetNextEmptyMailSlot()) == NULL)
	{
		enableInterrupts();
		return -2;
	}
	else
	{
		/* We got a slot, now we can copy the message */
		if (pMailBox->maxMessageSize < msg_size)
		{
			enableInterrupts();
			return -1;
		}

		CopyMessageToSlot(pMailSlot, pMsg, msg_size, myPid, mboxId, MS_STATUS_INUSE);

		/* If there is a process waiting to recv then deliver the message directly to them and wake them up */
		if (pMailBox->waitingProcsRecvList.count > 0)
		{
			pWaitingToRecvProcess = (MessagingProcess *)Pop(&pMailBox->waitingProcsRecvList);
			pWaitingToRecvProcess->pSlot = pMailSlot;
			pWaitingToRecvProcess->pSlot->status = MS_STATUS_DELIVERED_TO_PROC;
			UnblockMessagingProcess(pWaitingToRecvProcess->pid, MP_READY);
			result = 0;
		}
		else
		{
			if (pMailBox->deliveredMailList.count < pMailBox->slotCount)
			{
				pMailSlot->status = MS_STATUS_DELIVERED_TO_MBOX;
				InsertNode((void *)pMailSlot, &pMailBox->deliveredMailList);
				result = 0;
			}
			else if (wait)
			{
				/*block ourselves and wait for a slot to become available*/
				runningMessengerProcess->pSlot = pMailSlot;
				InsertNode((void *)runningMessengerProcess, &pMailBox->waitingProcsSendList);
				BlockMessagingProcess(myPid, MP_BLOCKED_SEND);

				/* After Awoken - Try to send again */
				disableInterrupts();
				runningMessengerProcess = FindProcessInTable(k_getpid());

				/* Handle a closing mailbox */
				result = HandleMailBoxClose(pMailBox);
				disableInterrupts();
				runningMessengerProcess = FindProcessInTable(k_getpid());
				/* If the process wasn't signaled and the mailbox wasn't closed, ensure the slot on the
				running process was reset */

				if (result != -5 && result != -3 && runningMessengerProcess->pSlot == NULL)
				{

					result = runningMessengerProcess->pSlot == NULL ? 0 : -1;
				}
				else
				{
					ResetMailSlot(runningMessengerProcess->pSlot);
					runningMessengerProcess->pSlot == NULL;
					enableInterrupts();
					return result;
				}
			}
			else
			{
				ResetMailSlot(pMailSlot);
				enableInterrupts();
				return -2;
			}
		}
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
int mailbox_receive(int mboxId, void *pMsg, int msg_size, int wait)
{
	checkKernelMode(__func__);
	runningMessengerProcess = FindProcessInTable(k_getpid());

	int result = -1;
	int signals = 0;
	MailBox *pMailBox;
	int myPid = k_getpid();
	MailSlot *pMailSlot = NULL;
	MessagingProcess *pWaitingToSendProcess;

	/* Validate the input */
	if (mboxId < 0 || msg_size < 0 || myPid < 0)
	{
		/* Invalid args, return -1 */
		return result;
	}

	disableInterrupts();
	/* Get the mailbox, any waiting to send processes and see if we have a message already delivered*/

	/* Ensure the mailbox exists */
	if (((pMailBox = &MAIL_BOXES[GetMailboxIdx(mboxId)]) == NULL) ||
		pMailBox->status == MB_STATUS_EMPTY ||
		pMailBox->status == MB_STATUS_RELEASED)
	{
		enableInterrupts();
		return -1;
	}

	if (pMailBox->waitingProcsSendList.count > 0)
	{
		pWaitingToSendProcess = (MessagingProcess *)Pop(&pMailBox->waitingProcsSendList);
		pMailSlot = pWaitingToSendProcess->pSlot;
		/*Zero slot mailbox*/
		if (pMailBox->slotCount == 0)
		{
			result = CopyMessageFromSlot(pMailSlot, pMsg, msg_size);

			UnblockMessagingProcess(pWaitingToSendProcess->pid, MP_READY);

			if ((signals = GetSignals(runningMessengerProcess)) == -3 || signals == -5)
			{
				enableInterrupts();
				return runningMessengerProcess->pSlot == NULL ? signals : -1;
			}
		}
		/* The mailbox has slots */
		else
		{
			/* Check to see if we can attach the message to the current process */
			if (runningMessengerProcess->pSlot == NULL ||
				(runningMessengerProcess->pSlot != NULL &&
				 runningMessengerProcess->pSlot->status == MS_STATUS_EMPTY))
			{
				/* The mailbox is full - but the current receiving process has an empty slot*/
				runningMessengerProcess->pSlot = pMailSlot;
				runningMessengerProcess->pSlot->status = MS_STATUS_DELIVERED_TO_PROC;

				/* Ensure the message can fit */
				if (msg_size < pMailSlot->messageSize)
				{
					enableInterrupts();
					return -1;
				}
				pWaitingToSendProcess->pSlot = NULL;

				/* Unblock the sender */
				UnblockMessagingProcess(pWaitingToSendProcess->pid, MP_READY);

				/* Handle a closing mailbox */

				disableInterrupts();
				result = GetSignals(runningMessengerProcess);
				runningMessengerProcess = FindProcessInTable(k_getpid());
				/* If the process wasn't signaled and the mailbox wasn't closed, ensure the slot on the
				running process was reset */

				if (result == -3 || result == -5)
				{
					enableInterrupts();
					return runningMessengerProcess->pSlot == NULL ? result : -1;
				}

				/* Add the process stored to the delivered list */
				InsertNode((void *)runningMessengerProcess->pSlot, &pMailBox->deliveredMailList);
				runningMessengerProcess->pSlot = NULL;

				/* Now we need to send back the next item in the delivery list*/
				pMailSlot = (MailSlot *)Pop(&pMailBox->deliveredMailList);
				result = CopyMessageFromSlot(pMailSlot, pMsg, msg_size);

				/* free the slot*/
				ResetMailSlot(pMailSlot);
			}
			else
			{
				/* The Mailbox is full we have no extra slots*/

				/* Now we need to send back the next item in the delivery list*/
				pMailSlot = (MailSlot *)Pop(&pMailBox->deliveredMailList);
				result = CopyMessageFromSlot(pMailSlot, pMsg, msg_size);

				/* free the slot*/
				ResetMailSlot(pMailSlot);

				/* Unblock the sender */
				UnblockMessagingProcess(pWaitingToSendProcess->pid, MP_READY);

				if ((signals = GetSignals(runningMessengerProcess)) == -3 || signals == -5)
				{
					enableInterrupts();
					return signals == -3 ? -1 : signals;
				}
			}
		}
	}
	else
	{
		/*
			WE have no waiting senders
		*/
		if (pMailBox->deliveredMailList.count > 0)
		{
			pMailSlot = (MailSlot *)Pop(&pMailBox->deliveredMailList);
			result = CopyMessageFromSlot(pMailSlot, pMsg, msg_size);
			runningMessengerProcess->pSlot = NULL;

			/* free the slot*/
			ResetMailSlot(pMailSlot);
		}
		else if (wait)
		{
			InsertNode((void *)runningMessengerProcess, &pMailBox->waitingProcsRecvList);
			BlockMessagingProcess(myPid, MP_BLOCKED_RECEIVE);

			/* After Awoken - CHECK TO SEE IF WE HAD A DIRECT DERIVERY */
			disableInterrupts();
			/* Handle a closing mailbox */
			result = HandleMailBoxClose(pMailBox);
			disableInterrupts();
			runningMessengerProcess = FindProcessInTable(k_getpid());

			/* If the process wasn't signaled and the mailbox wasn't closed, ensure the slot on the
			running process was reset */
			if (result == -3 || result == -5)
			{
				enableInterrupts();
				return runningMessengerProcess->pSlot == NULL ? result : -1;
			}

			result = CopyMessageFromSlot(runningMessengerProcess->pSlot, pMsg, msg_size);

			/* free the slot*/
			ResetMailSlot(runningMessengerProcess->pSlot);
			runningMessengerProcess->pSlot = NULL;
		}
	}

	enableInterrupts();
	return result;
}

/* ------------------------------------------------------------------------
   Name - MboxRelease
   ----------------------------------------------------------------------- */
int mailbox_free(int mboxId)
{
	checkKernelMode(__func__);
	enableInterrupts();

	MailBox *pBox;
	int result = -1;
	MessagingProcess *pProc;
	int waitingToCloseCount = 0;

	disableInterrupts();

	/* Get the mailbox in the mailbox table*/
	if ((pBox = &MAIL_BOXES[GetMailboxIdx(mboxId)]) == NULL)
	{
		enableInterrupts();
		return result;
	}

	/* Check for blockers, and wake them up*/
	DoublyLinkedList *pBlockedProcessLists[2] = {
		&pBox->waitingProcsSendList,
		&pBox->waitingProcsRecvList,

	};

	/* Mark all the processes as MP_BOX_DESTROYED */
	for (int i = 0; i < 2; i++)
	{
		MessagingProcess *pCurrent = (MessagingProcess *)pBlockedProcessLists[i]->pHead;

		while (pCurrent != NULL)
		{
			pProc = (MessagingProcess *)pCurrent;
			pProc->status = MP_BOX_DESTROYED;
			pCurrent = pCurrent->pNext;
			waitingToCloseCount++;
		}
	}

	pBox->closerPid = k_getpid();
	pBox->status = MB_STATUS_RELEASED;
	pBox->waitingToClose = waitingToCloseCount;

	/* Kill and Unblock any processes blocked on the mailbox */
	for (int i = 0; i < 2; i++)
	{
		while (pBlockedProcessLists[i]->count > 0)
		{
			pProc = (MessagingProcess *)Pop(pBlockedProcessLists[i]);
			k_kill(pProc->pid, SIG_TERM);
			UnblockMessagingProcess(pProc->pid, MP_BOX_DESTROYED);
		}
	}

	if (pBox->waitingToClose > 0)
	{
		/* If this list has any waiters,
		block and wait for the last one and then join it*/
		BlockMessagingProcess(k_getpid(), MP_BOX_DESTROYED);
		/* Unblock in case higher priority */
		UnblockMessagingProcess(pBox->closerPid, MP_BOX_DESTROYED);
		/* Try to join if lower */
		k_join(pBox->closerPid, &result);
		disableInterrupts();
		pBox->closerPid = k_getpid(); // reset the closer in case we are not the last list
	}

	disableInterrupts();
	ResetMailbox(pBox);
	enableInterrupts();

	return signaled() ? -5 : 0;
}

int wait_device(char *deviceName, int *status)
{
	int result = 0;
	uint32_t deviceHandle = -1;
	checkKernelMode(__func__);
	runningMessengerProcess = FindProcessInTable(k_getpid());
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
	return signaled() ? -1 : result;
}

int check_io_messaging(void)
{
	checkKernelMode(__func__);
	disableInterrupts();
	return waitingOnDevice ? 1 : 0;
}

/* an error method to handle invalid syscalls */
static void nullsys(system_call_arguments_t *args)
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
static inline void checkKernelMode(const char *functionName)
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
	handlers[THREADS_TIMER_INTERRUPT] = TimerInterruptHandler;
	handlers[THREADS_IO_INTERRUPT] = IOInterruptHandler;
	handlers[THREADS_SYS_CALL_INTERRUPT] = SystemCallInterruptHandler;

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

static void IOInterruptHandler(char deviceId[32], uint8_t command, uint32_t status)
{
	runningMessengerProcess = FindProcessInTable(k_getpid());

	LARGE_INTEGER unit;
	int result;
	int id;

	unit.QuadPart = (LONGLONG)deviceId;
	id = (int)unit.LowPart;

	if (deviceId >= THREADS_MAX_DEVICES)
	{
		console_output(FALSE, "IOInterruptHandler(): Device ID is out of range %d\n", deviceId);
		return;
	}

	result = mailbox_send(devices[id].deviceMbox, &status, sizeof(int), FALSE);
	if (result == -3 || result == -1)
	{
		console_output(FALSE, "IOInterruptHandler(): mailbox_send returned %d, pid = %d\n", result, k_getpid());
		stop(1);
	}

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
	static int count = 0;

	/* every 5th interrupt check for a message */
	if (count++ % 5 == 0)
	{
		/* If a process is waiting on a device */
		if (check_io_messaging())
		{
			/* get the mailbox for the clock */
			MailBox *pMailBox = &MAIL_BOXES[GetMailboxIdx(THREADS_CLOCK_DEVICE_ID)];

			/* Send the clock's status */
			mailbox_send(pMailBox->mboxId, &status, sizeof(int), FALSE);
		}
	}

	time_slice();
}

/*****************************************************************************
   Name - SystemCallInterruptHandler
   Purpose - Handles System Call interrupts
   Parameters -
   Returns -
****************************************************************************/

void SystemCallInterruptHandler(char deviceId[32], uint8_t command, uint32_t status)
{
	system_call_arguments_t *args = (system_call_arguments_t *)deviceId;

	if (args->call_id < 0 || args->call_id >= THREADS_MAX_SYSCALLS)
	{
		console_output(FALSE, "SystemCallInterruptHandler(): Invalid syscall %d\n", args->call_id);
		stop(1);
	}

	systemCallVector[args->call_id](args);
}
