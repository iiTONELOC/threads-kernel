#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct mailbox MailBox;
typedef struct mail_slot *SlotPtr;
typedef struct mbox_proc *mbox_proc_ptr;
typedef struct waiting_process *WaitingProcessPtr;

typedef enum
{
   MB_ZEROSLOT,
   MB_SINGLESLOT,
   MB_MULTISLOT,
   MB_MAXTYPES
} MAILBOX_TYPE;
typedef enum
{
   MB_STATUS_EMPTY,
   MB_STATUS_INUSE,
   MB_STATUS_RELEASED,
   MB_STATUS_MAX
} MAILBOX_STATUS;

typedef struct mail_slot
{
   int mbox_id;
   int messageSize;
   SlotPtr pNextSlot;
   SlotPtr pPrevSlot;
   unsigned char message[MAX_MESSAGE];
   /* other items as needed... */
} MailSlot;

struct mailbox
{
   int mbox_id;
   int slotCount;
   MAILBOX_TYPE type;
   int maxMessageSize;
   MAILBOX_STATUS status;
   SlotPtr pSlotListHead;
   /* other items as needed... */
};
#endif
