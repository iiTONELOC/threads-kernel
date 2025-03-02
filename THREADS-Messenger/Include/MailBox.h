#include "Messenger.h"

#pragma once
#ifndef MAIL_BOX_H
#define MAIL_BOX_H

// __________________________ Function Prototypes __________________________

void InitEmptyMailBoxList();
int GetMailboxIdx(int mboxId);
MailBox* GetNextEmptyMailbox();
void ResetMailbox(MailBox* pMailbox);
void ResetMailBoxSlots(MailBox* pMailBox);
void ResetMailBoxMsgProcs(MailBox* pMailBox);
int ReuseMailbox(MailBox* pMailbox, int slotCount, int slotSize);
#endif