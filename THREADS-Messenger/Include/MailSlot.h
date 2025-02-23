#pragma once

#ifndef MAIL_SLOT_H
#define MAIL_SLOT_H
#include "Messaging.h"
#include "DoublyLinkedList.h"
typedef struct mailSlot
{
    int toPid;
    int mboxId;
    int fromPid;
    int dynamic;
    void *pNext;
    void *pPrev;
    int messageSize;
    unsigned char message[MAX_MESSAGE];
    /* other items as needed... */
} MailSlot;

// ___________________________ Global Variables ___________________________

extern size_t NUM_M_SLOTS_IN_USE;             // Number of mailslots in use
extern MailSlot MAIL_SLOTS[MAXSLOTS];         // Array of mailslots
extern DoublyLinkedList MAIL_SLOT_EMPTY_LIST; // List of empty mailslots

// __________________________ Function Prototypes __________________________

void InitEmptyMailSlotList();
MailSlot *GetNextEmptyMailSlot();
void resetMailSlot(MailSlot *pMailSlot);
int reuseMailSlot(MailSlot *pMailSlot, int slotSize, int mboxId);

#endif
