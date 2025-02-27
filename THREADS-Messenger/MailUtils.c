#include "MailUtils.h"
#include <Windows.h>
#include <DoublyLinkedList.h>
#include "MailBox.h"
#include "MailSlot.h"
#include "Messaging.h"
#include "MessagingProcess.h"
#include "Messenger.h"
#include "Scheduler.h"
#include "THREADSLib.h"
#include <memory.h>
#include <string.h>

// _________________________________ Function Prototypes _________________________________

void CopyMessageToSlot(MailSlot *pSlot, void *pMsg, int msg_size, int pid, int mboxId, enum MAIL_SLOT_STATUS status);
// _________________________________ Function Definitions _________________________________

/**
 * @brief Initialize the tables
 *
 * This function initializes the tables
 */
void InitMessagingTables()
{
	InitEmptyMailSlotList();
	InitEmptyMailBoxList();
	InitEmptyMessagingProcessArray();
}

/**
 * @brief Trims the right side of a string
 *
 * @param pString Pointer to the string to trim
 */
void TrimRight(char *pString)
{
	int i = 0;
	// while we haven't reached the end of the string
	while (pString[i] != '\0')
	{
		// traverse to the end of the string
		i++;
	}

	// end of the string was found
	i--;

	// loop backwards until we find a character that is not a space, tab, newline, or carriage return
	while (pString[i] == ' ' || pString[i] == '\t' || pString[i] == '\n' || pString[i] == '\r')
	{
		// null terminate the String
		pString[i] = '\0';
		i--;
	}
}

/**
 * @brief Initialize system devices and their mailboxes
 *
 * This function initializes disk, terminal, clock, and interrupt devices
 *
 * @param pDevices A pointer to the devices to add mailboxes to
 */
void InitializeDevices(DeviceManagementData *pDevices)
{
	char *deviceNames[] = {"disk0", "disk1", "term0", "term1", "term2", "term3", "clock", "interrupt"};
	for (int i = 0; i < THREADS_MAX_DEVICES; ++i)
	{
		/* Copy the device name */
		CopyString(deviceNames[i], pDevices[i].deviceName, sizeof(pDevices[i].deviceName));
		/* Init the device and grab its handle */
		pDevices[i].deviceHandle = device_initialize(deviceNames[i]);
		if (i <= 1)
		{
			pDevices[i].deviceType = DEVICE_DISK;
		}
		else if (i < 6)
		{
			pDevices[i].deviceType = DEVICE_TERMINAL;
		}
		else if (i < 7)
		{
			pDevices[i].deviceType = DEVICE_CLOCK;
		}
		else
		{
			pDevices[i].deviceType = DEVICE_TERMINAL;
		}
		/* Create a mailbox for the device */
		pDevices[i].deviceMbox = mailbox_create(0, sizeof(int));
	}
}

/**
 * @brief Copy a string from source to destination
 *
 * @param pSource Pointer to the source string
 * @param pDestination Pointer to the destination string
 * @param size The size of the destination string
 */
void CopyString(char *pSource, char *pDestination, size_t size)
{
	// size_t is an unsigned long long integer
	size_t i = 0ULL;

	// Trim the right side of the string so we don't copy garbage
	TrimRight(pSource);

	// Traverse the source string, copying each character to the destination string
	// for a maximum of size - 1 characters
	while (pSource[i] != '\0' && i < size - 1)
	{
		// copy the character
		pDestination[i] = pSource[i];
		i++;
	}
	// ensure the destination string is null terminated
	pDestination[i] = '\0';
}

/**
 * @brief Block a messaging process
 *
 * This function blocks a messaging process with a given status
 *
 * @param pid The process id
 * @param status The status to block the process with
 *
 * @return 0 if successful, -1 if invalid args
 */
int BlockMessagingProcess(int pid, enum MESSAGING_PROCESS_STATUS status)
{
	int result = -1;

	MessagingProcess *pProcess;

	/* Validate the input */
	if (pid < 0 || status < 0)
	{
		return result;
	}

	/* validate  the process */
	pProcess = FindProcessInTable(pid, TRUE);

	/* If the process is null there is a problem */
	if (!pProcess)
	{
		return result;
	}

	/*Set its status to the status*/
	pProcess->status = status;
	/* set had to wait */
	pProcess->hadToWait = 1;

	/* Block the current process*/
	result = block(status);
	disableInterrupts();
	runningMessengerProcess = FindProcessInTable(pid, TRUE);
	return result;
}

/**
 * @brief Unblock a messaging process
 *
 * This function unblocks a messaging process with a given status
 *
 * @param pid The process id
 * @param status The status to unblock the process with
 *
 * @return 0 if successful, -1 if invalid args
 */
int UnblockMessagingProcess(int pid, enum MESSAGING_PROCESS_STATUS status)
{
	MessagingProcess *pProcess;

	/* Validate the input */
	if (pid < 0 || status < 0)
	{
		return -1;
	}

	/* validate the process */
	pProcess = FindProcessInTable(pid, TRUE);

	/* If the process is null there is a problem */
	if (!pProcess)
	{
		return -1;
	}

	/* Set the process status to ready */
	pProcess->status = status;

	/* Unblock the process */
	unblock(pid);
	disableInterrupts();
	runningMessengerProcess = FindProcessInTable(pid, TRUE);
	return 0;
}

/**
 * @brief Handle sending a message with zero slots
 *
 * This function handles sending a message with zero slots
 *
 * @param pMailBox A pointer to the mailbox
 * @param pProcess A pointer to the process
 * @param pMsg A pointer to the message
 * @param msg_size The size of the message
 * @param wait Whether or not to wait
 * @return 0 if successful, -1 if invalid args
 */
int HandleSendMailZeroSlots(MailBox *pMailBox, MessagingProcess *pProcess,
							void *pMsg, int msg_size, int wait)
{
	int result = -1;
	disableInterrupts();
	MailSlot *pSlot;

	if ((pSlot = GetNextEmptyMailSlot()) == NULL)
	{
		enableInterrupts();
		return result;
	}

	/* Copy the message into the slot */
	CopyMessageToSlot(pSlot, pMsg, msg_size, pProcess->pid, pMailBox->mboxId, MS_STATUS_INUSE);

	/* Look for a process to unblock */
	MessagingProcess *pProc = (MessagingProcess *)Pop(&pMailBox->waitingProcsRecvList);

	/* A process was found */
	if (pProc)
	{
		/* Copy the message into the process's slot */
		pSlot->status = MS_STATUS_DELIVERED_TO_PROC;
		pProc->pSlot = pSlot;

		/* Unblock the process */
		UnblockMessagingProcess(pProc->pid, MP_READY);
		result = msg_size;
	}
	else
	{
		if (wait)
		{
			InsertNode((void *)pProcess, &pMailBox->waitingProcsSendList);
			runningMessengerProcess = NULL;
			BlockMessagingProcess(pProcess->pid, MP_BLOCKED_SEND);

			/* After Awoken */
			pProcess = FindProcessInTable(pProcess->pid, TRUE);

			/*check if signaled*/
			if (pProcess->status == MP_BOX_DESTROYED)
			{
				enableInterrupts();
				return -3;
			}

			if (signaled())
			{
				enableInterrupts();
				return -5;
			}

			result = HandleSendMailZeroSlots(pMailBox, pProcess, pMsg, msg_size, wait);
		}
		else
		{
			/* Do not wait return immediately with a -2 */
			enableInterrupts();
			return -2;
		}
	}
	enableInterrupts();
	return msg_size;
}

/**
 * @brief Handle sending a message with slots
 *
 * This function handles sending a message with slots
 *
 * @param pMailBox A pointer to the mailbox
 * @param pProcess A pointer to the process
 * @param pMsg A pointer to the message
 * @param msg_size The size of the message
 * @param wait Whether or not to wait
 * @return 0 if successful, -1 if invalid args
 */
int HandleSendMailWithSlots(MailBox *pMailBox, MessagingProcess *pProcess,
							void *pMsg, int msg_size, int wait)
{
	int result = -1;
	disableInterrupts();
	int pid = k_getpid();
	MailSlot *pSlot = GetNextEmptyMailSlot();

	if (!pSlot)
	{
		enableInterrupts();
		return result;
	}

	/* Copy the message into the slot */
	CopyMessageToSlot(pSlot, pMsg, msg_size, pid,
					  pMailBox->mboxId, MS_STATUS_INUSE);

	/* If the mailbox is full - we cannot deliver */
	if (pMailBox->deliveredMailList.count == pMailBox->slotCount)
	{
		/* If we can wait */
		if (wait)
		{
			/*Place the slot in our pcb */
			pProcess->pSlot = pSlot;
			/* place ourselves into the mailbox's waiting to send list */
			InsertNode((void *)pProcess, &pMailBox->waitingProcsSendList);
			/* block ourselves */
			runningMessengerProcess = NULL;
			BlockMessagingProcess(pid, MP_BLOCKED_SEND);

			/* After Awoken */
			pProcess = FindProcessInTable(pid, TRUE);

			/*check if signaled*/
			if (pProcess->status == MP_BOX_DESTROYED)
			{
				enableInterrupts();
				return -3;
			}

			if (signaled())
			{
				enableInterrupts();
				return -5;
			}
		}
		/* Cannot wait, return -2 */
		else
		{
			enableInterrupts();
			return -2;
		}
	}

	/* Deliver the message to the mailbox */
	pSlot->status = MS_STATUS_DELIVERED_TO_MBOX;
	InsertNode((void *)pSlot, &pMailBox->deliveredMailList);
	result = 0;

	enableInterrupts();
	return result;
}

int HandleReceiveMailZeroSlots(MailBox *pMailBox, MessagingProcess *pProcess, void *pMsg, int msg_size, int wait)
{
	int result = -1;
	disableInterrupts();

	/* If there is no mail we cannot receive anything */
	if (!pProcess->pSlot || (pProcess->pSlot && pProcess->pSlot->status != MS_STATUS_DELIVERED_TO_PROC))
	{
		/* If we can wait */
		if (wait)
		{
			/* place ourselves in the mailbox's waiting to receive list */
			InsertNode((void *)pProcess, &pMailBox->waitingProcsRecvList);

			/* block ourselves */
			runningMessengerProcess = NULL;

			/* check if there is a process waiting to send on the mailbox */
			if (pMailBox->waitingProcsSendList.count > 0)
			{
				UnblockMessagingProcess(((MessagingProcess *)Pop(&pMailBox->waitingProcsSendList))->pid, MP_READY);
			}
			else
			{
				/*Block ourselves directly*/
				BlockMessagingProcess(pProcess->pid, MP_BLOCKED_RECEIVE);

				/* After Awoken - Try to receive again */
				/* update the pProcess data */

				pProcess = FindProcessInTable(k_getpid(), TRUE);
				/*check if signaled*/
				if (pProcess->status == MP_BOX_DESTROYED)
				{
					enableInterrupts();
					return -3;
				}

				if (signaled())
				{
					enableInterrupts();
					return -5;
				}
			}
		}
		/* Not allowed to block; return immediately with a -2 */
		else
		{
			enableInterrupts();
			return -2;
		}
	}

	/* Look for a message in the delivered list*/
	MailSlot *pSlot = pProcess->pSlot;

	/* Ensure the destination can hold the message before copying */
	if (msg_size < pSlot->messageSize)
	{
		enableInterrupts();
		return -1;
	}

	/* Copy the message into the pMsg buffer */
	memcpy_s(pMsg, msg_size, pSlot->message, pSlot->messageSize);

	/* Set result to num bytes copied */
	result = pSlot->messageSize;

	pProcess->pSlot = NULL;

	/* free the slot*/
	ResetMailSlot(pSlot);

	enableInterrupts();
	return result;
}

/**
 * @brief Handle receiving a message from a mailbox with slots
 *
 *
 * @param pMailBox A pointer to the mailbox
 * @param pProcess A pointer to the process
 * @param pMsg A pointer to the dest buffer
 * @param msg_size The size of the message
 * @param wait Whether or not to block
 * @return 0 if successful, -1 if invalid args
 */
int HandleReceiveMailWithSlots(MailBox *pMailBox, MessagingProcess *pProcess,
							   void *pMsg, int msg_size, int wait)
{
	int result = -1;
	disableInterrupts();

	/* If there is no mail we cannot receive anything */
	if (pMailBox->deliveredMailList.count == 0)
	{
		/* If we can wait */
		if (wait)
		{
			/* place ourselves in the mailbox's waiting to receive list */
			InsertNode((void *)pProcess, &pMailBox->waitingProcsRecvList);

			/* block ourselves */
			runningMessengerProcess = NULL;
			BlockMessagingProcess(pProcess->pid, MP_BLOCKED_RECEIVE);

			/* After Awoken - Try to receive again */
			/* update the pProcess data */
			pProcess = FindProcessInTable(runningMessengerProcess->pid, TRUE);
			if (pProcess->status == MP_BOX_DESTROYED)
			{
				enableInterrupts();
				return -3;
			}

			if (signaled())
			{
				enableInterrupts();
				return -5;
			}
			return HandleReceiveMailWithSlots(pMailBox, pProcess, pMsg, msg_size, wait);
		}
		/* Not allowed to block; return immediately with a -2 */
		else
		{
			enableInterrupts();
			return -2;
		}
	}

	/* Look for a message in the delivered list*/
	MailSlot *pSlot = (MailSlot *)Pop(&pMailBox->deliveredMailList);

	/* Ensure the destination can hold the message before copying */
	if (msg_size < pSlot->messageSize)
	{
		enableInterrupts();
		return -1;
	}

	/* Copy the message into the pMsg buffer */
	memcpy_s(pMsg, msg_size, pSlot->message, pSlot->messageSize);

	/* Set result to num bytes copied */
	result = pSlot->messageSize;

	/* free the slot*/
	ResetMailSlot(pSlot);

	enableInterrupts();
	return result;
}

void CopyMessageToSlot(MailSlot *pSlot, void *pMsg, int msg_size, int pid, int mboxId, enum MAIL_SLOT_STATUS status)
{
	/* Copy the message into the slot */
	memcpy_s(pSlot->message, sizeof(pSlot->message), pMsg, msg_size);
	pSlot->messageSize = msg_size;
	pSlot->fromPid = pid;
	pSlot->mboxId = mboxId;
	pSlot->status = status;
}
