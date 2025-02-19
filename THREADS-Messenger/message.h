

typedef struct waiting_process *WaitingProcessPtr;
typedef struct mail_slot *SlotPtr;
typedef struct mailbox MailBox;
typedef struct mbox_proc *mbox_proc_ptr;

typedef enum {MB_ZEROSLOT=0, MB_SINGLESLOT, MB_MULTISLOT, MB_MAXTYPES} MAILBOX_TYPE;
typedef enum {MBSTATUS_EMPTY=0, MBSTATUS_INUSE, MBSTATUS_RELEASED, MBSTATUS_MAX} MAILBOX_STATUS;

typedef struct mail_slot 
{
   SlotPtr   pNextSlot;
   SlotPtr   pPrevSlot;
   int       mbox_id;
   unsigned char message[MAX_MESSAGE];
   int       messageSize;
   /* other items as needed... */

} MailSlot;

struct mailbox 
{
   SlotPtr      pSlotListHead;
   int           mbox_id;
   /* other items as needed... */
   MAILBOX_TYPE      type;
   MAILBOX_STATUS    status;
   int               maxMessageSize;
   int               slotCount;
};



