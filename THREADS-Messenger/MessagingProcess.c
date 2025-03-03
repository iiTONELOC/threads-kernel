#include "MessagingProcess.h"
#include "Messenger.h"

MessagingProcess MESSAGING_PROCESSES[MAX_PROCESSES] = {0};

// _________________________________ Function Definitions _________________________________

/**
 * @brief Initialize a Linked List to Track Empty Messaging Processes
 */
void InitEmptyMessagingProcessArray()
{
	// loop over the array of messaging processes, ensuring they are all empty
	// but set the pid to the appropriate value, index + 1
	for (int i = 0; i < MAX_PROCESSES; i++)
	{
		ResetMessagingProcess(&MESSAGING_PROCESSES[i]);
		MESSAGING_PROCESSES[i].pid = i + 1;
		MESSAGING_PROCESSES[i].tableIndex = i;
	}
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

	MessagingProcess *pProcess = &MESSAGING_PROCESSES[(byPid % MAX_PROCESSES) - 1];

	pProcess->pid = byPid;
	runningMessengerProcess = pProcess;

	return pProcess;
}

/**
 * @brief Reset a messaging process
 *
 * This function resets a messaging process
 *
 * @param pMessagingProcess A pointer to the messaging process to reset
 */
void ResetMessagingProcess(MessagingProcess *pMessagingProcess)
{
	pMessagingProcess->pNext = NULL;
	pMessagingProcess->pPrev = NULL;
	pMessagingProcess->pSlot = NULL;
	pMessagingProcess->pSlot = NULL;
	pMessagingProcess->tableIndex = 0;
	pMessagingProcess->status = MP_STATUS_EMPTY;
}
