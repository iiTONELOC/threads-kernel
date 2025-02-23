#pragma once

#ifndef MAIL_BOX_H
#define MAIL_BOX_H
#include <string.h>
#include "Messaging.h"
#include "DoublyLinkedList.h"
#include "MailSlot.h"

typedef enum MAILBOX_STATUS
{
    MB_STATUS_EMPTY,
    MB_STATUS_INUSE,
    MB_STATUS_RELEASED,
    MB_STATUS_MAX // This is the number of mailbox statuses
} MAILBOX_STATUS;

typedef enum MB_LISTS
{
    MB_LISTS_EMPTY_SLOTS,
    MB_LISTS_WAITING_PROCS,
    MB_LISTS_DELIVERED_MAIL,
    MB_LISTS_MAX // This is the number of slot lists
} MB_LISTS;

typedef struct mailbox
{
    int mboxId;
    int slotCount;
    int maxMessageSize;
    MAILBOX_STATUS status;
    DoublyLinkedList slotLists[MB_LISTS_MAX];
} MailBox;

typedef struct mailSlot *SlotPtr;

// ___________________________ Global Variables ___________________________

extern size_t NUM_M_BOXES_IN_USE;            // Number of mailboxes in use
extern MailBox MAIL_BOXES[MAXMBOX];          // Array of mailboxes
extern DoublyLinkedList MAIL_BOX_EMPTY_LIST; // List of empty mailboxes

// __________________________ Function Prototypes __________________________

void InitEmptyMailBoxList();
MailBox *GetNextEmptyMailbox();
void resetMailbox(MailBox *pMailbox);
int reuseMailbox(MailBox *pMailbox, int slotCount, int slotSize);
#endif