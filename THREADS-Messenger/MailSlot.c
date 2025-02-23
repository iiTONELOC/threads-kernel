#include "THREADSLib.h"
#include "MailSlot.h"

size_t NUM_M_SLOTS_IN_USE = 0;
MailSlot MAIL_SLOTS[MAXSLOTS] = {0};
DoublyLinkedList MAIL_SLOT_EMPTY_LIST = {0};

/**
 * @brief Initialize a Linked List to Track Empty Mailslots
 */
void InitEmptyMailSlotList()
{
    /* Init the MAIL_SLOT_EMPTY_LIST List */
    InitializeDoublyLinkedList(FALSE, DOUBLY_LINKED_NODE_OFFSET, &MAIL_SLOT_EMPTY_LIST, NULL);

    /* Add all of the empty mailslots into the linked list */
    for (size_t i = 0; i < MAXSLOTS; i++)
    {
        /* Push the empty mailslot into the empty list */
        InsertNode((void *)&MAIL_SLOTS[i], &MAIL_SLOT_EMPTY_LIST);
    }
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
    DoublyLinkedNode *pNode;

    /* if the list is empty return NULL */
    if (MAIL_SLOT_EMPTY_LIST.count == 0 && NUM_M_SLOTS_IN_USE >= MAXSLOTS)
    {
        return NULL;
    }

    /* increment the number of mailslots in use */
    NUM_M_SLOTS_IN_USE++;

    /* Remove the node from the head of the list */
    pMailSlot = (MailSlot *)Pop(&MAIL_SLOT_EMPTY_LIST);

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
void resetMailSlot(MailSlot *pMailSlot)
{
    /* Reset the mailslot */
    memset(pMailSlot, 0, sizeof(MailSlot));

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
int reuseMailSlot(MailSlot *pMailSlot, int slotSize, int mboxId)
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
