
#include "MessagingProcess.h"

size_t NUM_MESSAGING_PROCESSES_IN_USE = 0;
DoublyLinkedList MESSAGING_PROCESS_EMPTY_LIST = {0};
MessagingProcess MESSAGING_PROCESSES[MAX_PROCESSES] = {0};

// _________________________________ Function Definitions _________________________________

/**
 * @brief Initialize a Linked List to Track Empty Messaging Processes
 */
void InitEmptyMessagingProcessList()
{
    InitStaticLinkedList(OFFSETOF_MSG_PROC, MAX_PROCESSES,
                         (void *)&MESSAGING_PROCESSES[0],
                         SIZEOF_MSG_PROC, OFFSETOF_MSG_PROC_TBL_IDX,
                         &MESSAGING_PROCESS_EMPTY_LIST, NULL);
}

/**
 * @brief Find a process in the table by pid
 *
 * This function finds a process in the table by pid
 *
 * @param byPid The pid to search for
 *
 * @return A pointer to the process if found, NULL otherwise
 */
MessagingProcess *FindProcessInTable(int byPid)
{
    /* Loop through the table to find the process */
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (MESSAGING_PROCESSES[i].pid == byPid)
        {
            return &MESSAGING_PROCESSES[i];
        }
    }

    return NULL;
}

/**
 * @brief Get the next empty messaging process
 *
 * This function gets the next empty messaging process from the list of empty processes
 *
 * @return A pointer to the next empty messaging process
 */
MessagingProcess *GetNextEmptyMessagingProcess()
{
    MessagingProcess *pMessagingProcess;

    /* Pop the next process off the list*/
    pMessagingProcess = (MessagingProcess *)Pop(&MESSAGING_PROCESS_EMPTY_LIST);

    if (!pMessagingProcess)
    {
        return NULL;
    }

    /* Increment the number of messaging processes in use */
    NUM_MESSAGING_PROCESSES_IN_USE++;

    /* Return the new message process*/
    return pMessagingProcess;
}

/**
 * @brief Reset a messaging process
 *
 * This function resets a messaging process
 *
 * @param pMessagingProcess A pointer to the messaging process to reset
 *
 * @note This function disables and enables interrupts before returning
 */
void ResetMessagingProcess(MessagingProcess *pMessagingProcess)
{
    disableInterrupts();

    /* Reset the messaging process - this leaves the table index - this won't change*/
    pMessagingProcess->pid = 0;
    pMessagingProcess->dynamic = 0;
    pMessagingProcess->pNext = NULL;
    pMessagingProcess->pPrev = NULL;
    pMessagingProcess->pSlot = NULL;
    pMessagingProcess->hadToWait = 0;
    pMessagingProcess->pMailBox = NULL;
    pMessagingProcess->status = MP_STATUS_EMPTY;

    /* Decrease number of processes */
    NUM_MESSAGING_PROCESSES_IN_USE--;

    /* Add it back to the empty list */
    InsertNode((void *)pMessagingProcess, &MESSAGING_PROCESS_EMPTY_LIST);

    enableInterrupts();
}

/**
 * @brief Reuse a messaging process
 *
 * This function reuses a messaging process
 *
 * @param pMessagingProcess A pointer to the messaging process to reuse
 * @param pid The pid of the process
 *
 * @return 0 if successful, -1 if invalid args
 */
int ReuseMessagingProcess(MessagingProcess *pMessagingProcess, int pid)
{
    /* Check for invalid args */
    if (!pMessagingProcess || pid < 0)
    {
        return -1;
    }

    /* Set pid */
    pMessagingProcess->pid;
    /* Set status to ready*/
    pMessagingProcess->status = MP_READY;

    return 0;
}