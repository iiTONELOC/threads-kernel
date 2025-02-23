#include "THREADSLib.h"
#include "MailBox.h"

size_t mailBoxId = 0;
size_t NUM_M_BOXES_IN_USE = 0;
MailBox MAIL_BOXES[MAXMBOX] = {0};
DoublyLinkedList MAIL_BOX_EMPTY_LIST = {0};

/**
 * @brief Initialize a Linked List to Track Empty Mailboxes
 */
void InitEmptyMailBoxList()
{
    /*Init the Mail Slots */
    InitEmptyMailSlotList();
    /* Init the MAIL_BOX_EMPTY_LIST List */
    InitializeDoublyLinkedList(FALSE, DOUBLY_LINKED_NODE_OFFSET, &MAIL_BOX_EMPTY_LIST, NULL);

    /* Add all of the empty mailboxes into the linked list */
    for (size_t i = 0; i < MAXMBOX; i++)
    {
        /* Push the empty mailbox into the empty list */
        InsertNode((void *)&MAIL_BOXES[i], &MAIL_BOX_EMPTY_LIST);
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
    DoublyLinkedNode *pNode;

    /* if the list is empty return NULL */
    if (MAIL_BOX_EMPTY_LIST.count == 0 && NUM_M_BOXES_IN_USE >= MAXMBOX)
    {
        return NULL;
    }

    /* Increment the mailbox id */
    mailBoxId++;

    /* increment the number of mailboxes in use
    not sure we need this*/
    NUM_M_BOXES_IN_USE++;

    /* Remove the node from the head of the list */
    pMailBox = (MailBox *)Pop(&MAIL_BOX_EMPTY_LIST);

    /* assign the new id to the mailbox */
    pMailBox->mboxId = mailBoxId;

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
void resetMailbox(MailBox *pMailbox)
{
    /* Reset the mailbox */
    memset(pMailbox, 0, sizeof(MailBox));

    /* Decrease the number of mailboxes in use */
    NUM_M_BOXES_IN_USE--;

    /* Add the mailbox to the empty list */
    InsertNode((void *)pMailbox, &MAIL_BOX_EMPTY_LIST);
}

/**
 * @brief Reuse a static mailbox
 *
 * 'Creates' a new mailbox using an empty mailbox from a static allocation
 *  of mailboxes.
 *
 * @param pMailbox A pointer to the mailbox to reuse
 * @param slotCount The number of slots in the mailbox
 * @param slotSize The number of bytes each slot can hold
 *
 * @return >= 0 Success, -1 if an error occurs
 */
int reuseMailbox(MailBox *pMailbox, int slotCount, int slotSize)
{
    int validSlots = slotCount > 0 && slotCount <= MAXSLOTS;
    int validSize = slotSize > 0 && slotSize <= MAX_MESSAGE;

    /*validate the input return -1 if invalid */
    if (!pMailbox || !validSlots || !validSize)
    {
        return -1;
    }

    /* Set the mailbox values */
    pMailbox->slotCount = slotCount;
    pMailbox->status = MB_STATUS_INUSE;
    pMailbox->maxMessageSize = slotSize;

    /* Initialize the Linked Lists */
    for (int i = 0; i < MB_LISTS_MAX; i++)
    {
        // the list will use MailSlots directly rather than a DoublyLinkedNode with a pData pointer
        InitializeDoublyLinkedList(FALSE, offsetof(MailSlot, pNext), &pMailbox->slotLists[i], NULL);
    }

    // for each slot in the mail box, add an empty slot to the empty list
    for (int i = 0; i < slotCount; i++)
    {
        MailSlot *pSlot = GetNextEmptyMailSlot();
        if (pSlot)
        {
            InsertNode((void *)pSlot, &pMailbox->slotLists[MB_LISTS_EMPTY_SLOTS]);
        }
    }

    return pMailbox->mboxId;
}
