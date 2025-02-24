#include "Messenger.h"
#include "MailBox.h"
#include "MailSlot.h"
#include "MessagingProcess.h"

#pragma once

#ifndef MAIL_UTILS_H
#define MAIL_UTILS_H

// _________________________________ Function Prototypes _________________________________

// int UnblockSendingProcess(int mboxId);
// int UnblockReceivingProcess(int mboxId);
void InitMessagingTables();

void InitDeviceMailBoxes(DeviceManagementData *pDevices);

int BlockSendingProcess(MessagingProcess *pProcess, MailBox *pMailBox);
int BlockReceivingProcess(MessagingProcess *pProcess, MailBox *pMailBox);
int UnblockSendingProcess(MessagingProcess *pProcess, MailBox *pMailBox);
int UnblockReceivingProcess(MessagingProcess *pProcess, MailBox *pMailBox);
int SendMail(MailBox *pMailBox, MailSlot *pSlot, void *pMsg, int msg_size, int myPid);

#endif
