#pragma once
#ifndef _USERPROCESS_H
#define _USERPROCESS_H
#include "_SystemCalls.h"

/* -------------------------------- Function Prototypes ------------------------------- */

/**
 * @brief Resets the user process to its initial state.
 *
 * @param pUserProc -  Pointer to the user process to reset.
 * @note this does not reset the tableIndex
 */
void ResetUserProcess(UserProcess *pUserProc);

#endif
