#include "Messenger.h"

#pragma once

#ifndef MAIL_UTILS_H
#define MAIL_UTILS_H

// _________________________________ Function Prototypes _________________________________

/**
 * @brief Initialize the tables
 *
 * This function initializes the tables
 */
void InitMessagingTables();

/**
 * @brief Initialize the device mailboxes
 *
 * This function initializes the device mailboxes
 *
 * @param pDevices A pointer to the devices to add mailboxes to
 */
void InitDeviceMailBoxes(DeviceManagementData* pDevices);

/**
 * @brief Block a messaging process
 *
 * This function blocks a messaging process with a given status
 *
 * @param pid The process id
 * @param status The status to block the process with
 *
 * @return 0 if successful, -1 if invalid args
 */
int BlockMessagingProcess(int pid, enum MESSAGING_PROCESS_STATUS status);

/**
 * @brief Unblock a messaging process
 *
 * This function unblocks a messaging process with a given status
 *
 * @param pid The process id
 * @param status The status to unblock the process with
 *
 * @return 0 if successful, -1 if invalid args
 */
int UnblockMessagingProcess(int pid, enum MESSAGING_PROCESS_STATUS status);

/**
 * @brief Handle sending a message with zero slots
 *
 * This function handles sending a message with zero slots
 *
 * @param pMailBox A pointer to the mailbox
 * @param pProcess A pointer to the process
 * @param pMsg A pointer to the message
 * @param msg_size The size of the message
 * @param wait Whether or not to wait
 * @return 0 if successful, -1 if invalid args
 */
int HandleSendMailZeroSlots(MailBox* pMailBox, MessagingProcess* pProcess, void* pMsg, int msg_size, int wait);
/**
 * @brief Handle sending a message with slots
 *
 * This function handles sending a message with slots
 *
 * @param pMailBox A pointer to the mailbox
 * @param pProcess A pointer to the process
 * @param pMsg A pointer to the message
 * @param msg_size The size of the message
 * @param wait Whether or not to wait
 * @return 0 if successful, -1 if invalid args
 */
int HandleSendMailWithSlots(MailBox* pMailBox, MessagingProcess* pProcess, void* pMsg, int msg_size, int wait);
int HandleReceiveMailZeroSlots(MailBox* pMailBox, MessagingProcess* pProcess, void* pMsg, int msg_size, int wait);
int HandleReceiveMailWithSlots(MailBox* pMailBox, MessagingProcess* pProcess, void* pMsg, int msg_size, int wait);

#endif
