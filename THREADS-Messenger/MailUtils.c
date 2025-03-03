#include "MailUtils.h"
#include <Windows.h>
#include "MailBox.h"
#include "MailSlot.h"
#include "Messaging.h"
#include "MessagingProcess.h"
#include "Messenger.h"
#include "Scheduler.h"
#include "THREADSLib.h"
#include <memory.h>

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

int GetSignals(MessagingProcess *pProcess)
{
	return (pProcess->status == MP_BOX_DESTROYED) ? -3 : (signaled() ? -5 : 0);
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
	pProcess = FindProcessInTable(pid);

	/* If the process is null there is a problem */
	if (!pProcess)
	{
		return result;
	}

	/*Set its status to the status*/
	pProcess->status = status;

	/* Block the current process*/
	result = block(status);
	disableInterrupts();
	runningMessengerProcess = FindProcessInTable(pid);
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
	pProcess = FindProcessInTable(pid);

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
	runningMessengerProcess = FindProcessInTable(pid);
	return 0;
}

/**
 * @brief Copy the message from a slot to a buffer
 *
 * @param pSlot A pointer to the slot to copy the message from
 * @param pBuffer A pointer to the buffer to copy the message to
 *
 * @return The number of bytes copied or -1 if an error occurs
 */
int CopyMessageFromSlot(MailSlot *pSlot, void *pBuffer, int buffSize)
{
	if (!pSlot || !pBuffer || buffSize < 0 || buffSize < pSlot->messageSize)
	{
		return -1;
	}

	/* Copy the message into the buffer */
	memcpy_s(pBuffer, buffSize, pSlot->message, pSlot->messageSize);

	return pSlot->messageSize;
}

/**
 * @brief Copy the message from a buffer to a slot
 *
 * @param pSlot A pointer to the slot to copy the message to
 * @param pBuffer A pointer to the buffer to copy the message from
 * @param buffSize The size of the buffer
 */
void CopyMessageToSlot(MailSlot *pSlot, void *pMsg, int msg_size, int pid, int mboxId, enum MAIL_SLOT_STATUS status)
{

	/* Copy the message into the slot */
	memcpy_s(pSlot->message, sizeof(pSlot->message), pMsg, msg_size);
	pSlot->messageSize = msg_size;
	pSlot->fromPid = pid;
	pSlot->mboxId = mboxId;
	pSlot->status = status;
}
