#include "MailUtils.h"

// _________________________________ Function Definitions _________________________________

/**
 * @brief Initialize the tables
 *
 * This function initializes the tables
 */
void InitMessagingTables()
{
    InitEmptyMailSlotList();
    InitEmptyMailBoxList();
    InitEmptyMessagingProcessList();
}

/**
 * @brief Initialize the device mailboxes
 *
 * This function initializes the device mailboxes
 *
 * @param pDevices A pointer to the devices to add mailboxes to
 */
void InitDeviceMailBoxes(DeviceManagementData *pDevices)
{
    for (int i = 0; i < THREADS_MAX_DEVICES; ++i)
    {
        pDevices[i].deviceMbox = mailbox_create(0, sizeof(int));
    }
}

/**
 * @brief Block a sending process
 *
 * This function blocks a sending process
 *
 * @param pProcess A pointer to the process
 * @param pMailBox A pointer to the mailbox
 *
 * @return 0 if successful, -1 if invalid args
 * @note This function disables interrupts but re-enable behavior depends on the block function
 */
int BlockSendingProcess(MessagingProcess *pProcess, MailBox *pMailBox)
{

    /* If the process is null there is a problem */
    if (!pProcess)
    {
        return -1;
    }

    disableInterrupts();
    /* Set the process' mailbox id to the mailbox id */
    pProcess->pMailBox = pMailBox;
    /*Set its status to blocked send*/
    pProcess->status = MP_BLOCKED_SEND;
    /* set had to wait */
    pProcess->hadToWait = 1;

    /* Add the Waiting Process to the List on the Mailbox*/
    InsertNode((void *)pProcess, &pMailBox->waitingProcsSendList);

    /* Block the current process*/
    return block(pProcess->status);
}

/**
 * @brief Block a receiving process
 *
 * This function blocks a receiving process
 *
 * @param pProcess A pointer to the process
 * @param pMailBox A pointer to the mailbox
 *
 * @return 0 if successful, -1 if invalid args
 * @note This function disables interrupts but re-enable behavior depends on the block function
 */
int BlockReceivingProcess(MessagingProcess *pProcess, MailBox *pMailBox)
{
    /* If the process is null there is a problem */
    if (!pProcess)
    {
        return -1;
    }

    disableInterrupts();
    /* Set the process' mailbox id to the mailbox id */
    pProcess->pMailBox = pMailBox;
    /*Set its status to blocked receive*/
    pProcess->status = MP_BLOCKED_RECEIVE;
    /* set had to wait */
    pProcess->hadToWait = 1;

    /* Add the Waiting Process to the List on the Mailbox*/
    InsertNode((void *)pProcess, &pMailBox->waitingProcsRecvList);

    /* Block the current process*/
    return block(pProcess->status);
}

/**
 * @brief Unblock a sending process
 *
 * This function unblocks a sending process
 *
 * @param pProcess A pointer to the process
 * @param pMailBox A pointer to the mailbox
 *
 * @return 0 if successful, -1 if invalid args
 * @note This function disables interrupts and enables them before returning
 */
int UnblockSendingProcess(MessagingProcess *pProcess, MailBox *pMailBox)
{
    /* If the process is null there is a problem */
    if (!pProcess)
    {
        return -1;
    }

    disableInterrupts();
    /* Remove the process from the waiting list */
    RemoveNode((void *)pProcess, &pMailBox->waitingProcsSendList);
    /* Set the process status to ready */
    pProcess->status = MP_READY;
    enableInterrupts();

    /* Unblock the process */
    return unblock(pProcess->pid);
}

/**
 * @brief Unblock a receiving process
 *
 * This function unblocks a receiving process
 *
 * @param pProcess A pointer to the process
 * @param pMailBox A pointer to the mailbox
 *
 * @return 0 if successful, -1 if invalid args
 * @note This function disables interrupts and enables them before returning
 */
int UnblockReceivingProcess(MessagingProcess *pProcess, MailBox *pMailBox)
{
    /* If the process is null there is a problem */
    if (!pProcess)
    {
        return -1;
    }

    disableInterrupts();
    /* Remove the process from the waiting list */
    RemoveNode((void *)pProcess, &pMailBox->waitingProcsRecvList);
    /* Set the process status to ready */
    pProcess->status = MP_READY;
    enableInterrupts();

    /* Unblock the process */
    return unblock(pProcess->pid);
}

/**
 * @brief Send a message to a mailbox
 *
 * This function sends a message to a mailbox
 *
 * @param pMailBox A pointer to the mailbox
 * @param pSlot A pointer to the mailslot
 * @param pMsg A pointer to the message
 * @param msg_size The size of the message
 * @param myPid The pid of the process sending the message
 *
 * @return 0 if successful, -1 if invalid args or error occurred
 *
 * @note This function disables interrupts and enables them before returning
 */
int SendMail(MailBox *pMailBox, MailSlot *pSlot, void *pMsg, int msg_size, int myPid)
{
    int result = -1;

    /*  SET STATUS TO IN USE  */
    disableInterrupts();
    /*set the status of the mailbox to in_use*/
    pMailBox->status = MB_STATUS_INUSE;
    /*set the status of the slot to in use*/
    pSlot->status = MS_STATUS_INUSE;
    enableInterrupts();

    /* TRANSFER MESSAGE */
    disableInterrupts();
    /* Copy the message into the slot*/
    memcpy_s(pSlot->message, MAX_MESSAGE, pMsg, msg_size);

    /* Ensure the message is null terminated */
    pSlot->message[(msg_size < MAX_MESSAGE) ? (msg_size) : (MAX_MESSAGE - 1)] = '\0';

    pSlot->messageSize = msg_size;

    /* Set the from pid to the current process' pid*/
    pSlot->fromPid = myPid;
    enableInterrupts();

    /* DELIVER MAIL */
    disableInterrupts();
    /* Remove the node from the slots list */
    RemoveNode((void *)pSlot, &pMailBox->mailSlotsList);

    /* Place the slot in the delivered mail list*/
    pSlot->status = MS_STATUS_DELIVERED;
    InsertNode((void *)pSlot, &pMailBox->deliveredMailList);

    /*Set the Mailbox status to ready*/
    pMailBox->status = MB_STATUS_READY;
    enableInterrupts();

    /* Look for a process to unblock from the block_proc_recv ? */
    if (pMailBox->waitingProcsRecvList.count > 0)
    {
        MessagingProcess *pBlockedOnRecv = (MessagingProcess *)Pop(&pMailBox->waitingProcsRecvList);

        if (pBlockedOnRecv)
        {
            /*Unblock the process*/
            disableInterrupts();
            pBlockedOnRecv->status = MP_READY;
            /* Do we care about return val here?*/
            result = unblock(pBlockedOnRecv->pid);
            /*Ensure that we are not preempted - they are enabled elsewhere*/
            disableInterrupts();
            /* if we were signaled while blocked */
            result = (signaled() && pBlockedOnRecv->hadToWait) ? (-5) : (result);
        }
        else
        {
            console_output(0, "SendMail: pBlockedOnRecv is NULL But receive list is not empty!\n");
            result = -1;
        }
    }
    else
    {
        result = 0;
    }
    enableInterrupts();
    return result;
}
