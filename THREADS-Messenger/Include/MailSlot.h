#include "Messenger.h"
#pragma once
#ifndef MAIL_SLOT_H
#define MAIL_SLOT_H

// __________________________ Function Prototypes __________________________

void InitEmptyMailSlotList();
MailSlot *GetNextEmptyMailSlot();
void ResetMailSlot(MailSlot *pMailSlot);
int ReuseMailSlot(MailSlot *pMailSlot, int slotSize, int mboxId);

#endif
