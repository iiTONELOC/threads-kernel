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
 * @brief Trims the right side of a string
 *
 * @param pString Pointer to the string to trim
 */
void TrimRight(char *pString);

int GetSignals(MessagingProcess *pProcess);

/**
 * @brief Initialize the device mailboxes
 *
 * This function initializes the device mailboxes
 *
 * @param pDevices A pointer to the devices to add mailboxes to
 */
void InitializeDevices(DeviceManagementData *pDevices);

/**
 * @brief Copy a string from source to destination
 *
 * @param pSource Pointer to the source string
 * @param pDestination Pointer to the destination string
 * @param size The size of the destination string
 */
void CopyString(char *pSource, char *pDestination, size_t size);

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
 * @brief Copy the message from a slot to a buffer
 *
 * @param pSlot A pointer to the slot to copy the message from
 * @param pBuffer A pointer to the buffer to copy the message to
 *
 * @return The number of bytes copied or -1 if an error occurs
 */
int CopyMessageFromSlot(MailSlot *pSlot, void *pBuffer, int buffSize);

/**
 * @brief Copy the message from a buffer to a slot
 *
 * @param pSlot A pointer to the slot to copy the message to
 * @param pBuffer A pointer to the buffer to copy the message from
 * @param buffSize The size of the buffer
 */
void CopyMessageToSlot(MailSlot *pSlot, void *pMsg, int msg_size, int pid, int mboxId, enum MAIL_SLOT_STATUS status);

#endif
