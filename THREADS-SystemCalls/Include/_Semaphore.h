#pragma once
#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include "_SystemCalls.h"

/* -------------------------------------- Prototypes ----------------------------------- */

/**
 * @brief Initializes the semaphore table and its empty list.
 *
 * @param pSemTable - Pointer to the semaphore table to initialize.
 * @param pFreeList - Pointer to the free list to initialize.
 */
void InitializeSemTable(SemData *pSemTable, DSL_List *pFreeList);

#endif
