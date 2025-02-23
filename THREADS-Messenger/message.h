#ifndef MESSAGE_H
#define MESSAGE_H
#include "MailBox.h"

typedef struct mailbox MailBox;
typedef struct mailSlot *SlotPtr;
typedef struct mbox_proc *mbox_proc_ptr;
typedef struct waiting_process *WaitingProcessPtr;

typedef struct messagingProcess
{
   int pid;
   int dynamic;
   void *pNextSlot;
   void *pPrevSlot;
} MessagingProcess;

#endif
