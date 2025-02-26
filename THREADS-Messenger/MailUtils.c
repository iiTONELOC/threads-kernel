#include "MailUtils.h"

// _________________________________ Function Prototypes _________________________________

void CopyMessageToSlot(MailSlot *pSlot, void *pMsg, int msg_size, int pid, int mboxId, enum MAIL_SLOT_STATUS status);
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
    InitEmptyMessagingProcessArray();
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
 * @brief Block a messaging process
 *
 * This function blocks a messaging process with a given status
 *
 * @param mboxId The mailbox id
 * @param pid The process id
 * @param status The status to block the process with
 *
 * @return 0 if successful, -1 if invalid args
 */
int BlockMessagingProcess(int mboxId, int pid, enum MESSAGING_PROCESS_STATUS status)
{
    int result = -1;
    MailBox *pMailBox;
    MessagingProcess *pProcess;

    /* Validate the input */
    if (mboxId < 0 || pid < 0 || status < 0)
    {
        return result;
    }

    /* Get the mailbox */
    pMailBox = &MAIL_BOXES[GetMailboxIdx(mboxId)];

    /* Get the process */
    pProcess = FindProcessInTable(pid, FALSE);

    /* If the process is null there is a problem */
    if (!pProcess)
    {
        return result;
    }

    /* Set the process' mailbox id to the mailbox id */
    // pProcess->pMailBox = pMailBox;
    /*Set its status to the status*/
    pProcess->status = status;
    /* set had to wait */
    pProcess->hadToWait = 1;

    /* Add the Waiting Process to the List on the Mailbox*/
    if (status == MP_BLOCKED_SEND)
    {
        InsertNode((void *)pProcess, &pMailBox->waitingProcsSendList);
    }
    else if (status == MP_BLOCKED_RECEIVE)
    {
        InsertNode((void *)pProcess, &pMailBox->waitingProcsRecvList);
    }

    /* Block the current process*/
    runningMessengerProcess = NULL;
    enableInterrupts();
    result = block(pProcess->status);
    disableInterrupts();
    return result;
}

/**
 * @brief Unblock a messaging process
 *
 * This function unblocks a messaging process with a given status
 *
 * @param mboxId The mailbox id
 * @param pid The process id
 * @param status The status to unblock the process with
 *
 * @return 0 if successful, -1 if invalid args
 */
int UnblockMessagingProcess(int mboxId, int pid, enum MESSAGING_PROCESS_STATUS status)
{
    MailBox *pMailBox;
    MessagingProcess *pProcess;

    /* Validate the input */
    if (mboxId < 0 || pid < 0 || status < 0)
    {
        return -1;
    }

    /* Get the mailbox */
    pMailBox = &MAIL_BOXES[GetMailboxIdx(mboxId)];

    /* Get the process */
    pProcess = FindProcessInTable(pid, FALSE);

    /* If the process is null there is a problem */
    if (!pProcess)
    {
        return -1;
    }

    /* Remove the process from the waiting list */
    if (status == MP_BLOCKED_SEND)
    {
        RemoveNode((void *)pProcess, &pMailBox->waitingProcsSendList);
    }
    else if (status == MP_BLOCKED_RECEIVE)
    {
        RemoveNode((void *)pProcess, &pMailBox->waitingProcsRecvList);
    }
    /* Set the process status to ready */
    pProcess->status = MP_READY;

    /* Unblock the process */
    enableInterrupts();
    unblock(pProcess->pid);
    disableInterrupts();
    return 0;
}

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
int HandleSendMailZeroSlots(MailBox *pMailBox, MessagingProcess *pProcess, void *pMsg, int msg_size, int wait)
{
    /*
    a. If the mailbox has no slots
             - Look for a process waiting to receive a message
                 - Copy the message into the process's slot
                 - Unblock the process
             - If no process is waiting, block the sending process
  */
    int result = -1;
    disableInterrupts();
    MailSlot *pSlot = GetNextEmptyMailSlot();
    if (!pSlot)
    {
        enableInterrupts();
        return result;
    }

    /* Copy the message into the slot */
    CopyMessageToSlot(pSlot, pMsg, msg_size, pProcess->pid, pMailBox->mboxId, MS_STATUS_INUSE);

    /* Look for a process to unblock */
    MessagingProcess *pProc = (MessagingProcess *)Pop(&pMailBox->waitingProcsRecvList);

    /* A process was found */
    if (pProc)
    {
        /* Copy the message into the process's slot */
        pSlot->status = MS_STATUS_DELIVERED_TO_PROC;
        pProc->pSlot = pSlot;

        /* Unblock the process */
        UnblockMessagingProcess(pMailBox->mboxId, pProc->pid, MP_READY);
        result = msg_size;
    }
    else
    {

        if (wait)
        {
            /* Add the sending process to the waiting list */
            pProcess->status = MP_BLOCKED_SEND;
            InsertNode((void *)pProcess, &pMailBox->waitingProcsSendList);

            /* Block the sending process */
            runningMessengerProcess = NULL;
            result = block(pProcess->status);

            /* After Awoken */

            result = HandleSendMailZeroSlots(pMailBox, pProcess, pMsg, msg_size, wait);
        }
        else
        {
            /* Do not wait return immediately with a -2 */
            enableInterrupts();
            return -2;
        }
    }
    enableInterrupts();
    return msg_size;
}

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
int HandleSendMailWithSlots(MailBox *pMailBox, MessagingProcess *pProcess, void *pMsg, int msg_size, int wait)
{
    /*
     b. If the mailbox has slots
                - Look to see if the slot can be delivered to the mailbox
                - If the slot can be delivered, deliver the message to the mailbox
                    - Unblock any processes waiting to receive a message
                - If the slot cannot be delivered
                    - Add the sending process to the waiting list
                    - Block the sending process
    */
    int result = -1;
    disableInterrupts();
    MailSlot *pSlot = GetNextEmptyMailSlot();
    if (!pSlot)
    {
        enableInterrupts();
        return result;
    }

    /* Copy the message into the slot */
    CopyMessageToSlot(pSlot, pMsg, msg_size, pProcess->pid, pMailBox->mboxId, MS_STATUS_INUSE);

    /* Look to see if the Mailbox is full*/
    if (pMailBox->deliveredMailList.count == pMailBox->slotCount)
    {
        if (wait)
        {
            /* Add the sending process to the waiting list */
            pProcess->status = MP_BLOCKED_SEND;
            InsertNode((void *)pProcess, &pMailBox->waitingProcsSendList);
            /* Block the sending process */
            runningMessengerProcess = NULL;
            block(pProcess->status);

            /* After Awoken */

            result = HandleSendMailWithSlots(pMailBox, pProcess, pMsg, msg_size, wait);
        }
        else
        {
            result = -2;
        }
    }
    else
    {
        /* Deliver the message to the mailbox */
        pSlot->status = MS_STATUS_DELIVERED_TO_MBOX;
        InsertNode((void *)pSlot, &pMailBox->deliveredMailList);

        /* Unblock any processes waiting to receive a message */
        MessagingProcess *pProc = (MessagingProcess *)Pop(&pMailBox->waitingProcsRecvList);
        if (pProc)
        {
            pProc->pSlot = pSlot;
            result = UnblockMessagingProcess(pMailBox->mboxId, pProc->pid, MP_READY);
        }

        result = 0;
    }
    enableInterrupts();

    return result;
}

int HandleReceiveMailZeroSlots(MailBox *pMailBox, MessagingProcess *pProcess, void *pMsg, int msg_size, int wait)
{
    return -1;
}

int HandleReceiveMailWithSlots(MailBox *pMailBox, MessagingProcess *pProcess, void *pMsg, int msg_size, int wait)
{
    int result = -1;
    disableInterrupts();
    /* If the mailbox has slots*/
    /*
        1. Look in the mailbox for a message
        2. If there is a message, copy it to the pMsg pointer
            3. Unblock any processes waiting to send a message
        4. If there is no message, block the receiving process
    */

    /* Look for a message in the delivered list*/
    MailSlot *pSlot = (MailSlot *)Pop(&pMailBox->deliveredMailList);

    if (pSlot)
    {
        /* Copy the message into the pMsg buffer */
        memcpy_s(pMsg, msg_size, pSlot->message, pSlot->messageSize);
        result = pSlot->messageSize;

        /* free the slot*/
        ResetMailSlot(pSlot);

        /* Unblock any processes waiting to sent*/
        MessagingProcess *pProc = (MessagingProcess *)Pop(&pMailBox->waitingProcsSendList);

        if (pProc)
        {
            UnblockMessagingProcess(pMailBox->mboxId, pProc->pid, MP_READY);
        }
    }
    else
    {
        /* No messages in the delivered list*/
        if (wait)
        {
            /* block the current process*/
            pProcess->status = MP_BLOCKED_RECEIVE;
            InsertNode((void *)pProcess, &pMailBox->waitingProcsRecvList);
            runningMessengerProcess = NULL;
            block(pProcess->status);

            /* After Awoken */
            result = HandleReceiveMailWithSlots(pMailBox, pProcess, pMsg, msg_size, wait);
        }
        else
        {
            enableInterrupts();
            return -2;
        }
    }

    enableInterrupts();
    return result;
}

void CopyMessageToSlot(MailSlot *pSlot, void *pMsg, int msg_size, int pid, int mboxId, enum MAIL_SLOT_STATUS status)
{
    /* Copy the message into the slot */
    memcpy(pSlot->message, pMsg, msg_size);
    pSlot->messageSize = msg_size;
    pSlot->fromPid = pid;
    pSlot->mboxId = mboxId;
    pSlot->status = status;
}