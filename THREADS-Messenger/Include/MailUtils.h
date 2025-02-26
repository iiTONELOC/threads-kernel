#include "Messenger.h"
#include "MailBox.h"
#include "MailSlot.h"
#include "MessagingProcess.h"

#pragma once

#ifndef MAIL_UTILS_H
#define MAIL_UTILS_H

// _________________________________ Function Prototypes _________________________________

void InitMessagingTables();

void InitDeviceMailBoxes(DeviceManagementData *pDevices);

int BlockMessagingProcess(int mboxId, int pid, enum MESSAGING_PROCESS_STATUS status);
int UnblockMessagingProcess(int mboxId, int pid, enum MESSAGING_PROCESS_STATUS status);
int HandleSendMailZeroSlots(MailBox *pMailBox, MessagingProcess *pProcess, void *pMsg, int msg_size, int wait);
int HandleSendMailWithSlots(MailBox *pMailBox, MessagingProcess *pProcess, void *pMsg, int msg_size, int wait);
int HandleReceiveMailZeroSlots(MailBox *pMailBox, MessagingProcess *pProcess, void *pMsg, int msg_size, int wait);
int HandleReceiveMailWithSlots(MailBox *pMailBox, MessagingProcess *pProcess, void *pMsg, int msg_size, int wait);

#endif
