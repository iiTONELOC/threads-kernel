#include "_Semaphore.h"

/* -------------------------- Globals ------------------------------------- */

int sys_semCount = 0;     /* number of semaphores in use */
static int nextSemId = 0; /* next semaphore id to use */

/* ------------------------- Definitions ----------------------------------- */

/**
 * @brief Resets the semaphore data structure to its initial state.
 *
 * @param pSem - Pointer to the semaphore data structure to reset.
 * @note This function sets the semaphore count to 0 and marks it as free.
 *       But does not free the mailboxes associated with the semaphore.
 *       It is assumed that the semaphore is not in use when this function is called.
 *       This function decrements the semaphore count.
 */
void ResetSem(SemData *pSem)
{
    if (pSem == NULL)
    {
        return; /* Invalid semaphore pointer */
    }

    pSem->count = 0;
    pSem->semId = -1;
    pSem->pNextSem = NULL;
    pSem->pPrevSem = NULL;
    pSem->status = SEM_FREE;
    /* don't clean nodes, this will reset any processes waiting */
    DSL_DestroyList(&pSem->waitingProcs, 0);
    DSL_InitList(0, OFFSETOF_USER_PROC_NEXT_NODE, &pSem->waitingProcs, NULL);
    sys_semCount--;
}

/**
 * @brief Initializes a semaphore data structure.
 *
 * @note This function initializes the semaphore data structure with default values.
 *
 * @param pSem - Pointer to the semaphore data structure to initialize.
 */
void InitializeSem(SemData *pSem)
{
    /* Initialize the semaphore data structure with default values */
    pSem->count = 0;
    pSem->semId = -1;
    pSem->mutexId = -1;
    pSem->tableIndex = -1;
    pSem->pNextSem = NULL;
    pSem->pPrevSem = NULL;
    pSem->status = SEM_FREE;
    pSem->privateMboxId = -1;

    /* Initialize the waiting processes list */
    DSL_InitList(0, OFFSETOF_USER_PROC_NEXT_NODE, &pSem->waitingProcs, NULL);
}

/**
 * @brief Gets the next empty semaphore index from the semaphore table.
 *
 * @param pSemTable - Pointer to the semaphore table.
 * @param size - Size of the semaphore table.
 * @return The index of the next empty semaphore or -1 if none found.
 * @note This function searches the semaphore table for an empty semaphore.
 */
int GetNextEmptySemIndex(SemData *pSemTable, int size)
{
    if (pSemTable == NULL || size <= 0 || size > MAX_SEMS || sys_semCount > MAX_SEMS)
    {
        return -1; /* Invalid semaphore table or size */
    }

    /* Search for the next empty semaphore in the semaphore table */
    for (int i = 0; i < size; i++)
    {
        if (pSemTable[i].status == SEM_FREE)
        {
            return i;
        }
    }
    return -1; /* No empty semaphore found */
}

/**
 * @brief Gets the next empty semaphore from the semaphore table.
 *
 * @param pSemTable - Pointer to the semaphore table.
 * @param size - Size of the semaphore table.
 * @return Pointer to the next empty semaphore or NULL if none found.
 * @note This function will increment the semaphore ID, sys_semCount and return the
 *       next empty semaphore from the table.
 */
SemData *GetNextEmptySemaphore(SemData *pSemTable, int size)
{
    if (pSemTable == NULL || size <= 0 || size > MAX_SEMS || sys_semCount > MAX_SEMS)
    {
        return NULL; /* Invalid semaphore table or size */
    }

    /* Get the next empty semaphore index */
    int index = GetNextEmptySemIndex(pSemTable, size);
    if (index < 0)
    {
        return NULL; /* No empty semaphore found */
    }

    /* Increment the semaphore ID, status and return it */
    pSemTable[index].semId = nextSemId++;
    pSemTable[index].status = SEM_IN_USE;
    sys_semCount++;
    return &pSemTable[index];
}

/**
 * @brief Initializes the semaphore data structure with the given parameters.
 *
 * @note This function initializes the semaphore data structure with the specified ID,
 * count, and status. It also creates a private mailbox for the semaphore.
 *
 * @param pSem - Pointer to the semaphore data structure to initialize.
 * @param sem_id - Semaphore ID.
 * @param count - Initial count of the semaphore.
 * @param status - Status of the semaphore (in use or not).
 */
void InitializeSemWData(SemData *pSem, int sem_id, int count, int status)
{
    InitializeSem(pSem);
    pSem->semId = sem_id;
    pSem->count = count;
    pSem->status = status;
    pSem->privateMboxId = mailbox_create(1, sizeof(int));
}
