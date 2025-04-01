#pragma once
#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include "_SystemCalls.h"

int sys_semCount;

/* -------------------------------------- Prototypes ----------------------------------- */

/**
 * @brief Resets the semaphore data structure to its initial state.
 *
 * @param pSem - Pointer to the semaphore data structure to reset.
 * @note This function sets the semaphore count to 0 and marks it as free.
 *       But does not free the mailboxes associated with the semaphore.
 *       It is assumed that the semaphore is not in use when this function is called.
 */
void ResetSem(SemData *pSem);

/**
 * @brief Initializes a semaphore data structure.
 *
 * @param pSem - Pointer to the semaphore data structure to initialize.
 */
void InitializeSem(SemData *pSem);

/**
 * @brief Gets the next empty semaphore index from the semaphore table.
 *
 * @param pSemTable - Pointer to the semaphore table.
 * @param size - Size of the semaphore table.
 * @return The index of the next empty semaphore or -1 if none found.
 * @note This function searches the semaphore table for an empty semaphore.
 */
int GetNextEmptySemIndex(SemData *pSemTable, int size);

/**
 * @brief Gets the next empty semaphore from the semaphore table.
 *
 * @param pSemTable - Pointer to the semaphore table.
 * @param size - Size of the semaphore table.
 * @return Pointer to the next empty semaphore or NULL if none found.
 * @note This function will increment the semaphore ID and return the
 *       next empty semaphore from the table.
 */
SemData *GetNextEmptySemaphore(SemData *pSemTable, int size);
/**
 * @brief Initializes the semaphore data structure with the given parameters.
 *
 * @param pSem - Pointer to the semaphore data structure to initialize.
 * @param sem_id - Semaphore ID.
 * @param count - Initial count of the semaphore.
 * @param status - Status of the semaphore (in use or not).
 */
void InitializeSemWData(SemData *pSem, int sem_id, int count, int status);

#endif
