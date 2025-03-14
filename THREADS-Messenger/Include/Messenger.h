#include <string.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "DoubleSeaLib.h"
#include <stddef.h>

#pragma once
#ifndef MESSENGER_H
#define MESSENGER_H
#define MESSAGING_QUANTUM 100000 // 100ms

typedef enum MAIL_SLOT_STATUS
{
	MS_STATUS_EMPTY,
	MS_STATUS_INUSE,
	MS_STATUS_DELIVERED_TO_MBOX,
	MS_STATUS_DELIVERED_TO_PROC,
	MS_STATUS_RELEASED,
	MS_STATUS_MAX // This is the number of mailslot statuses
} MAIL_SLOT_STATUS;

typedef enum MAILBOX_STATUS
{
	MB_STATUS_EMPTY,
	MB_STATUS_READY,
	MB_STATUS_INUSE,
	MB_STATUS_RELEASED,
	MB_STATUS_MAX // This is the number of mailbox statuses
} MAILBOX_STATUS;

typedef enum MESSAGING_PROCESS_STATUS
{
	MP_STATUS_EMPTY = 0,
	MP_READY = 1,
	MP_RUNNING = 2,
	// Has to be larger than 10 for use with the block function
	MP_BLOCKED_SEND = 11,
	MP_BLOCKED_RECEIVE = 12,
	MP_BOX_DESTROYED = 13,
	// -----------------------------------------------------------
	MP_STATUS_MAX = 5 // This is the number of messaging process statuses
} MESSAGING_PROCESS_STATUS;

typedef struct
{
	void *deviceHandle;
	int deviceMbox;
	int deviceType;
	char deviceName[16];
} DeviceManagementData;

typedef struct mailSlot
{
	int mboxId;
	int fromPid;
	int dynamic;
	void *pNext;
	void *pPrev;
	int tableIndex;
	int messageSize;
	enum MAIL_SLOT_STATUS status;
	unsigned char message[MAX_MESSAGE];
	/* other items as needed... */
} MailSlot;

typedef struct messagingProcess
{
	int pid;
	void *pNext;
	void *pPrev;
	int tableIndex;
	MailSlot *pSlot;
	enum MESSAGING_PROCESS_STATUS status;
} MessagingProcess;

typedef struct mailbox
{
	int mboxId;
	int dynamic;
	void *pNext;
	void *pPrev;
	int closerPid;
	int slotCount;
	int tableIndex;
	int procsWaitingToClose;
	int maxMessageSize;
	MAILBOX_STATUS status;
	DSL_List deliveredMailList;
	DSL_List waitingProcsRecvList;
	DSL_List waitingProcsSendList;
} MailBox;

typedef struct mailSlot *SlotPtr;

#define SIZEOF_MBOX sizeof(MailBox)
#define OFFSETOF_MBOX offsetof(MailBox, pNext)
#define OFFSETOF_MBOX_TBL_IDX offsetof(MailBox, tableIndex)

#define SIZEOF_MSLOT sizeof(MailSlot)
#define OFFSETOF_MSLOT offsetof(MailSlot, pNext)
#define OFFSETOF_MSLOT_TBL_IDX offsetof(MailSlot, tableIndex)

#define SIZEOF_MSG_PROC sizeof(MessagingProcess)
#define OFFSETOF_MSG_PROC offsetof(MessagingProcess, pNext)
#define OFFSETOF_MSG_PROC_TBL_IDX offsetof(MessagingProcess, tableIndex)
// ___________________________ Global Variables ___________________________
extern MessagingProcess *runningMessengerProcess;			// The current running process
extern MailBox MAIL_BOXES[MAXMBOX];							// Array of mailboxes
extern DSL_List MBOX_EMPTY_LIST;							// List of empty mailboxes
extern size_t NUM_M_SLOTS_IN_USE;							// Number of mailslots in use
extern MailSlot MAIL_SLOTS[MAXSLOTS];						// Array of mailslots
extern DSL_List MAIL_SLOT_EMPTY_LIST;						// List of empty mailslots
extern MessagingProcess MESSAGING_PROCESSES[MAX_PROCESSES]; // Array of messaging processes
#endif
