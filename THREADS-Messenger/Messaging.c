/* ------------------------------------------------------------------------
   Messaging.c
   College of Applied Science and Technology
   The University of Arizona
   CYBV 489

   Student Names: Anthony Tropeano, Connor Stackhouse

   ------------------------------------------------------------------------ */
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <THREADSLib.h>
#include <Scheduler.h>
#include <Messaging.h>
#include <Messenger.h>
#include <MailBox.h>
#include <MailSlot.h>
#include <MessagingProcess.h>
#include <MailUtils.h>

/* ------------------------- Prototypes ----------------------------------- */
static void nullsys(system_call_arguments_t *args);

typedef void (*interrupt_handler_t)(char deviceId[32], uint8_t command, uint32_t status);

static void InitializeHandlers();
static int check_io_messaging(void);
extern int MessagingEntryPoint(void *);
static void checkKernelMode(const char *functionName);

struct psr_bits
{
    unsigned int cur_int_enable : 1;
    unsigned int cur_mode : 1;
    unsigned int prev_int_enable : 1;
    unsigned int prev_mode : 1;
    unsigned int unused : 28;
};

union psr_values
{
    struct psr_bits bits;
    unsigned int integer_part;
};

/* -------------------------- Globals ------------------------------------- */

/* Obtained from THREADS*/
interrupt_handler_t *handlers;

/* system call array of function pointers */
void (*systemCallVector[THREADS_MAX_SYSCALLS])(system_call_arguments_t *args);

static int inSetup = 1;
static int waitingOnDevice = 0;
static DeviceManagementData devices[THREADS_MAX_DEVICES];

/* ------------------------------------------------------------------------
     Name - SchedulerEntryPoint
     Purpose - Initializes mailboxes and interrupt vector.
               Start the Messaging test process.
     Parameters - one, default arg passed by k_spawn that is not used here.
----------------------------------------------------------------------- */
int SchedulerEntryPoint(void *arg)
{
    int result = 0;
    // check for kernel mode
    checkKernelMode(__func__);

    /* Disable interrupts */
    disableInterrupts();

    /* set this to the real check_io function. */
    check_io = check_io_messaging;

    /* Initialize the mail box table, slots, & other data structures.
     * Initialize int_vec and sys_vec, allocate mailboxes for interrupt
     * handlers.  Etc... */
    InitMessagingTables();

    /* Initialize the devices and their mailboxes. */
    /* Allocate mailboxes for use by the interrupt handlers */
    InitDeviceMailBoxes(devices);

    InitializeHandlers();

    inSetup = 0;
    enableInterrupts();

    /* Create a process for Messaging, then block on a wait until messaging exits.*/
    result = k_spawn("MessagingEntryPoint", MessagingEntryPoint,
                     NULL, 4 * THREADS_MIN_STACK_SIZE, HIGHEST_PRIORITY);

    if (result < 0)
    {
        console_output(FALSE, "SchedulerEntryPoint(): spawn for MessagingEntryPoint returned an error (%d), stopping...\n", result);
        stop(1);
    }

    /* wait for the MessagingEntryPoint to finish */
    k_wait(&result);

    k_exit(result);

    return 0;
} /* SchedulerEntryPoint */

/* ------------------------------------------------------------------------
   Name - mailbox_create
   Purpose - gets a free mailbox from the table of mailboxes and initializes it
   Parameters - maximum number of slots in the mailbox and the max size of a msg
                sent to the mailbox.
   Returns - -1 to indicate that no mailbox was created, or a value >= 0 as the
             mailbox id.
   ----------------------------------------------------------------------- */
int mailbox_create(int slots, int slot_size)
{
    checkKernelMode(__func__);
    int result = -1;

    if (!inSetup)
        disableInterrupts();

    result = ReuseMailbox(GetNextEmptyMailbox(), slots, slot_size);

    if (!inSetup)
        enableInterrupts();

    return result;
} /* mailbox_create */

/* ------------------------------------------------------------------------
   Name - mailbox_send
   Purpose - Put a message into a slot for the indicated mailbox.
             Block the sending process if no slot available.
   Parameters - mailbox id, pointer to data of msg, # of bytes in msg.
   Returns - zero if successful, -1 if invalid args.
   Side Effects - none.
   ----------------------------------------------------------------------- */
int mailbox_send(int mboxId, void *pMsg, int msg_size, int wait)
{
    checkKernelMode(__func__);
    /* Producer */
    int result = -1;
    MailSlot *pSlot;
    MailBox *pMailBox;
    int myPid = k_getpid();
    MessagingProcess *pProcess;

    /* Validate the input*/
    if (mboxId < 0 || !pMsg || msg_size < 0 || myPid < 0)
    {
        /* Invalid args, return -1 */
        return result;
    }

    /* Grab the mailbox and create a new messenger process for the current process */
    disableInterrupts();
    if ((pMailBox = &MAIL_BOXES[GetMailboxIdx(mboxId)]) == NULL ||
        (pProcess = GetNextEmptyMessagingProcess()) == NULL)
    {
        /* Invalid mailbox, return -1 */
        enableInterrupts();
        return result;
    }

    /* Try to acquire a slot if the mailbox has room */
    if ((pMailBox->mailSlotsList.count + // check if the mailbox is full
         pMailBox->deliveredMailList.count) >= pMailBox->slotCount)
    {
        /* We have to block, no slots are available */

        /* Explicitly told not to block. Do not wait, return -2 */
        if (!wait)
        {
            ResetMessagingProcess(pProcess);
            enableInterrupts();
            return -2;
        }

        /* Block the process */
        result = BlockSendingProcess(pProcess, pMailBox);

        /* AFTER UNBLOCK - ensure interrupts are disabled */
        disableInterrupts();

        /* Try to send the message again, a slot should have opened up */
        return mailbox_send(mboxId, pMsg, msg_size, wait);
    }

    /* Slots are available for the mailbox */

    /* Try to get a slot from the slot table */
    if ((pSlot = GetNextEmptyMailSlot()) == NULL)
    {
        enableInterrupts();
        return -1;
    }

    /* Associate the node with the mail box and add it to its slot list */
    pSlot->mboxId = mboxId;
    InsertNode((void *)pSlot, &pMailBox->mailSlotsList);

    /* Send mail - disables interrupts and enables them before returning */
    result = SendMail(pMailBox, pSlot, pMsg, msg_size, myPid);

    /* See if any processes are waiting to be awoken*/
    if (pMailBox->waitingProcsRecvList.count > 0)
    {
        /* Unblock the first process in the list */
        UnblockReceivingProcess(
            (MessagingProcess *)Pop(&pMailBox->waitingProcsRecvList), pMailBox);
    }

    /* Reset the process - Disables and enables interrupts */
    ResetMessagingProcess(pProcess);

    return result;
}

/* ------------------------------------------------------------------------
   Name - mailbox_receive
   Purpose - Put a message into a slot for the indicated mailbox.
             Block the sending process if no slot available.
   Parameters - mailbox id, pointer to data of msg, # of bytes in msg.
   Returns - zero if successful, -1 if invalid args.
   Side Effects - none.
   ----------------------------------------------------------------------- */
int mailbox_receive(int mboxId, void *pMsg, int msg_size, int wait)
{
    checkKernelMode(__func__);
    /*Consumer*/
    int result = -1;
    MailSlot *pSlot;
    MailBox *pMailBox;
    int myPid = k_getpid();
    MessagingProcess *pProcess;

    /* Validate the input */
    if (mboxId < 0 || !pMsg || msg_size < 0 || myPid < 0)
    {
        /* Invalid args, return -1 */
        return result;
    }

    /* Grab the mailbox and create a new messenger process for the current process */
    disableInterrupts();
    if ((pMailBox = &MAIL_BOXES[GetMailboxIdx(mboxId)]) == NULL ||
        (pProcess = GetNextEmptyMessagingProcess()) == NULL)
    {
        /* Invalid mailbox, return -1 */
        enableInterrupts();
        return result;
    }

    /* Try to receive a message if there is a message to be received, if not we have to block*/
    if (pMailBox->deliveredMailList.count == 0)
    {
        /* No mail has been delivered, we need to block the current process*/

        if (!wait)
        {
            ResetMessagingProcess(pProcess);
            enableInterrupts();
            return -2;
        }

        /* Block the process */
        result = BlockReceivingProcess(pProcess, pMailBox);

        /* AFTER UNBLOCK - ensure interrupts are disabled */
        disableInterrupts();

        /* Try to receive the message again, a message should have been delivered */
        return mailbox_receive(mboxId, pMsg, msg_size, wait);
    }

    /* There is a message to be received */

    /* Get the first message in the delivered mail list */
    pSlot = (MailSlot *)Pop(&pMailBox->deliveredMailList);

    /* Copy the message into the buffer */
    memcpy_s(pMsg, msg_size, pSlot->message, pSlot->messageSize);

    /* set the result to the message size if not signaled*/

    if (signaled() && pProcess->hadToWait)
    {
        result = -5;
    }
    else
    {
        result = pSlot->messageSize;
    }

    /* Unblock the next proccess if needed */
    if (pMailBox->waitingProcsSendList.count > 0)
    {
        UnblockSendingProcess(
            (MessagingProcess *)Pop(&pMailBox->waitingProcsSendList), pMailBox);
    }

    /* Destroy the message process */
    ResetMessagingProcess(pProcess);

    return result;
}

/* ------------------------------------------------------------------------
   Name - MboxRelease
   ----------------------------------------------------------------------- */
int mailbox_free(int mboxId)
{
    checkKernelMode(__func__);
    int result = -1;

    return result;
}

int wait_device(char *deviceName, int *status)
{

    int result = 0;
    uint32_t deviceHandle = -1;
    checkKernelMode("waitdevice");

    enableInterrupts();

    if (strcmp(deviceName, "clock") == 0)
    {
        deviceHandle = THREADS_CLOCK_DEVICE_ID;
        ;
    }
    else
    {
        deviceHandle = device_handle(deviceName);
    }

    if (deviceHandle >= 0 && deviceHandle < THREADS_MAX_DEVICES)
    {
        /* set a flag that there is a process waiting on a device. */
        waitingOnDevice++;
        mailbox_receive(devices[deviceHandle].deviceMbox, status, sizeof(int), TRUE);
        waitingOnDevice--;
    }
    else
    {
        console_output(FALSE, "Unknown device type.");
        stop(-1);
    }

    /* spec says return -1 if zapped. */
    if (signaled())
    {
        result = -5;
    }

    return result;
}

int check_io_messaging(void)
{
    checkKernelMode(__func__);
    if (waitingOnDevice)
    {
        return 1;
    }
    return 0;
}

static void InitializeHandlers()
{
    checkKernelMode(__func__);
    handlers = get_interrupt_handlers();
}

/* an error method to handle invalid syscalls */
static void nullsys(system_call_arguments_t *args)
{
    console_output(FALSE, "nullsys(): Invalid syscall %d. Halting...\n", args->call_id);
    stop(1);
} /* nullsys */

/*****************************************************************************
   Name - checkKernelMode
   Purpose - Checks the PSR for kernel mode and halts if in user mode
   Parameters -
   Returns -
****************************************************************************/
static inline void checkKernelMode(const char *functionName)
{
    union psr_values psrValue;

    psrValue.integer_part = get_psr();
    if (psrValue.bits.cur_mode == 0)
    {
        console_output(FALSE, "Kernel mode expected, but function called in user mode.\n");
        stop(1);
    }
}
