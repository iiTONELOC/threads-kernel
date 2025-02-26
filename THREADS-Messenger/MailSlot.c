#include "MailSlot.h"

size_t NUM_M_SLOTS_IN_USE = 0;
MailSlot MAIL_SLOTS[MAXSLOTS] = {0};
DoublyLinkedList MAIL_SLOT_EMPTY_LIST = {0};

/**
 * @brief Initialize a Linked List to Track Empty Mailslots
 */
void InitEmptyMailSlotList()
{
    InitStaticLinkedList(OFFSETOF_MSLOT, MAXSLOTS, (void *)&MAIL_SLOTS,
                         SIZEOF_MSLOT, OFFSETOF_MSLOT_TBL_IDX,
                         &MAIL_SLOT_EMPTY_LIST, NULL);
}

/**
 * @brief Get the next empty mailslot
 *
 * This function gets the next empty mailslot from the list of empty mailslots
 *
 * @return A pointer to the next empty mailslot
 */

MailSlot *GetNextEmptyMailSlot()
{
    MailSlot *pMailSlot;

    /* if the list is empty return NULL */
    if (MAIL_SLOT_EMPTY_LIST.count == 0 && NUM_M_SLOTS_IN_USE >= MAXSLOTS)
    {
        return NULL;
    }

    /* increment the number of mailslots in use */
    NUM_M_SLOTS_IN_USE++;

    /* Remove the node from the head of the list */
    pMailSlot = (MailSlot *)Pop(&MAIL_SLOT_EMPTY_LIST);

    /* if the mail slot is null stop(1)*/
    if (!pMailSlot)
    {
        console_output(FALSE, "GetNextEmptyMailSlot: pMailSlot is NULL - System is out of empty mail slots!\n");
        stop(1);
    }

    /* return the pointer to the mailslot */
    return pMailSlot;
}

/**
 * @brief Reset a MailSlot
 *
 * This function resets a mailslot to its default values
 *
 * @param pMailSlot A pointer to the mailslot to reset
 */
void ResetMailSlot(MailSlot *pMailSlot)
{
    /* Reset the mailslot */
    pMailSlot->mboxId = 0;
    pMailSlot->fromPid = 0;
    pMailSlot->dynamic = 0;
    pMailSlot->pNext = NULL;
    pMailSlot->pPrev = NULL;
    pMailSlot->messageSize = 0;
    pMailSlot->status = MS_STATUS_EMPTY;
    memset(pMailSlot->message, 0, MAX_MESSAGE);
    /* Decrease the number of slots in use*/
    NUM_M_SLOTS_IN_USE--;

    /* Add the mailslot to the empty list */
    InsertNode((void *)pMailSlot, &MAIL_SLOT_EMPTY_LIST);
}

/**
 * @brief Reuse a MailSlot
 *
 * This function 'Creates' a mailslot by using a static mailslot from a static
 * array of mailslots
 *
 * @param pMailSlot A pointer to the mailslot to reuse
 * @param slotSize The size of the mailslot
 * @param mboxId The mailbox id
 * @return 0 if successful, -1 if invalid args
 */
int ReuseMailSlot(MailSlot *pMailSlot, int slotSize, int mboxId)
{
    /* Check for invalid arguments */
    if (pMailSlot == NULL || slotSize < 0 || slotSize > MAX_MESSAGE || mboxId < 0)
    {
        return -1;
    }

    /* Set the mailbox id */
    pMailSlot->mboxId = mboxId;

    /* Set the message size */
    pMailSlot->messageSize = slotSize;

    /* Return success */
    return 0;
}
