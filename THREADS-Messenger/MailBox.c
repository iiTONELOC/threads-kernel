#include "MailBox.h"

// #include "MailUtils.h"

size_t mailBoxId = 0;
MailBox MAIL_BOXES[MAXMBOX] = {0};
DoublyLinkedList MBOX_EMPTY_LIST = {0};

// __________________________ Function Prototypes __________________________

void _IncrementMailBoxId();

/**
 * @brief Initialize a Linked List to Track Empty Mailboxes
 */
void InitEmptyMailBoxList()
{
    /* Init the Linked List Structure */
    InitStaticLinkedList(OFFSETOF_MBOX, MAXMBOX, (void *)&MAIL_BOXES,
                         SIZEOF_MBOX, OFFSETOF_MBOX_TBL_IDX,
                         &MBOX_EMPTY_LIST, NULL);
}

/**
 * @brief Get the mailbox's index
 *
 * This function gets the mailbox index in the mailbox table
 *
 * @param mboxId The mailbox id
 *
 * @return The mailbox index or -1 if the mailbox id is invalid
 */
int GetMailboxIdx(int mboxId)
{
    /* If the mailbox id is invalid, return -1 */
    if (mboxId < 0 || mboxId >= MAXMBOX)
    {
        return -1;
    }
    /* If the mailbox id is greater than the max mailbox id, return the modulus
     of the max mailbox id */
    else if (mboxId > MAXMBOX)
    {
        return (mboxId % MAXMBOX) - 1;
    }
    /* Otherwise, return the mailbox id */
    else
    {
        return mboxId - 1;
    }
}

/**
 * @brief Get the next empty mailbox
 *
 * This function gets the next empty mailbox from the list of empty mailboxes
 *
 * @return A pointer to the next empty mailbox
 */
MailBox *GetNextEmptyMailbox()
{
    MailBox *pMailBox;

    /* Remove the node from the head of the list */
    pMailBox = (MailBox *)Pop(&MBOX_EMPTY_LIST);

    if (!pMailBox)
        return NULL;

    /* Increment the mailbox id */
    _IncrementMailBoxId();

    /* assign the new id to the mailbox */
    pMailBox->mboxId = (int)mailBoxId;

    /* return the pointer to the mailbox */
    return pMailBox;
}

/**
 * @brief Reset a mail box
 *
 * This function resets a mail box to "0" or Null values
 *
 * @param pMailbox A pointer to the mailbox to rest
 */
void ResetMailbox(MailBox *pMailbox)
{
    /* Reset the mailbox's slots*/
    ResetMailBoxSlots(pMailbox);

    /* Reset the mailbox's messaging processes */
    ResetMailBoxMsgProcs(pMailbox);

    /* Reset the remaining values */
    pMailbox->mboxId = 0;
    pMailbox->dynamic = 0;
    pMailbox->pNext = NULL;
    pMailbox->pPrev = NULL;
    pMailbox->slotCount = 0;
    pMailbox->maxMessageSize = 0;
    pMailbox->status = MB_STATUS_EMPTY;
    memset(&pMailbox->mailSlotsList, 0, sizeof(DoublyLinkedList));
    memset(&pMailbox->deliveredMailList, 0, sizeof(DoublyLinkedList));
    memset(&pMailbox->waitingProcsSendList, 0, sizeof(DoublyLinkedList));
    memset(&pMailbox->waitingProcsRecvList, 0, sizeof(DoublyLinkedList));

    /* Add the mailbox to the empty list */
    InsertNode((void *)pMailbox, &MBOX_EMPTY_LIST);
}

/**
 * @brief Reset a MailBox's slots
 *
 * This function resets all the mailslots in a mailbox.
 * This is necessary so that the slots are placed back on the empty list and
 * counts are updated.
 *
 * @param pMailBox A pointer to the mailbox
 */
void ResetMailBoxSlots(MailBox *pMailBox)
{
    /* List of our slots */
    DoublyLinkedList slotLists[] = {pMailBox->mailSlotsList, pMailBox->deliveredMailList};

    /* Reset any mailslots, they could be anywhere in the lists*/
    for (int i = 0; i < sizeof(slotLists) / sizeof(slotLists[0]); i++)
    {
        MailSlot *pSlot = (MailSlot *)Pop(&slotLists[i]);
        while (pSlot)
        {
            ResetMailSlot(pSlot);
            pSlot = (MailSlot *)Pop(&slotLists[i]);
        }
    }
}

/**
 * @brief Reset a MailBox's messaging processes
 *
 * This function resets all the messaging processes in a mailbox.
 * This is necessary so that the processes are placed back on the empty list and
 * counts are updated.
 *
 * @param pMailBox A pointer to the mailbox
 */
void ResetMailBoxMsgProcs(MailBox *pMailBox)
{
    /* List of messaging processes */
    DoublyLinkedList listsToCheck[] = {pMailBox->waitingProcsSendList, pMailBox->waitingProcsRecvList};

    /*Reset any messaging processes, they could be anywhere in the waiting lists*/
    for (int i = 0; i < sizeof(listsToCheck) / sizeof(listsToCheck[0]); i++)
    {
        MessagingProcess *pMsgProc = (MessagingProcess *)Pop(&listsToCheck[i]);
        while (pMsgProc)
        {
            ResetMessagingProcess(pMsgProc);
            pMsgProc = (MessagingProcess *)Pop(&listsToCheck[i]);
        }
    }
}

/**
 * @brief Reuse a static mailbox
 *
 * 'Creates' a new mailbox using an empty mailbox from a static allocation.
 *
 * @param pMailbox A pointer to the mailbox to reuse
 * @param slotCount The number of slots in the mailbox
 * @param slotSize The number of bytes each slot can hold
 *
 * @return >= 0 Success, -1 if an error occurs
 */
int ReuseMailbox(MailBox *pMailbox, int slotCount, int slotSize)
{
    int validSlots = slotCount >= 0 && slotCount <= MAXSLOTS;
    int validSize = slotSize >= 0 && slotSize <= MAX_MESSAGE;

    /*validate the input return -1 if invalid */
    if (!pMailbox || !validSlots || !validSize)
    {
        return -1;
    }

    /* Set the mailbox values - the mboxid is already assigned*/
    pMailbox->slotCount = slotCount;     // set the number of slots in the mailbox
    pMailbox->status = MB_STATUS_READY;  // set the mailbox status to ready
    pMailbox->maxMessageSize = slotSize; // set the max message size

    /* Initialize the Linked Lists */
    InitializeDoublyLinkedList(FALSE, OFFSETOF_MSLOT, &pMailbox->mailSlotsList, NULL);
    InitializeDoublyLinkedList(FALSE, OFFSETOF_MSLOT, &pMailbox->deliveredMailList, NULL);
    InitializeDoublyLinkedList(FALSE, OFFSETOF_MSG_PROC, &pMailbox->waitingProcsSendList, NULL);
    InitializeDoublyLinkedList(FALSE, OFFSETOF_MSG_PROC, &pMailbox->waitingProcsRecvList, NULL);

    return pMailbox->mboxId;
}

/**
 * @brief Increment the mailbox id
 *
 * This function increments the mailbox id
 */
void _IncrementMailBoxId()
{
    /* If we have rolled over*/
    if ((mailBoxId > MAXMBOX) && ((mailBoxId % MAXMBOX) == 0))
    {
        /* Ensure that we account for the device's mailboxes */
        mailBoxId = mailBoxId + THREADS_MAX_DEVICES;
    }
    /* Have not rolled over, increment normally */
    else
    {
        mailBoxId++;
    }
}