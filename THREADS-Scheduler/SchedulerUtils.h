#pragma once
#ifndef SCHEDULER_UTILS_H
#define SCHEDULER_UTILS_H
#include <stdlib.h>
#include "Constants.h"

/*_______________________Function Prototypes_______________________*/

/**
 * @brief Trims the right side of a string
 *
 * @param pString The string to trimS
 */
void TrimRight(char *pString);

/**
 * @brief Copy a string from source to destination
 *
 * @param pSource The source string
 * @param pDestination The destination string
 * @param size The size of the destination string
 */
void CopyString(char *pSource, char *pDestination, size_t size);

int ValidateKSpawnParams(char *name, int (*entryPoint)(void *), void *arg, int stacksize,
                         int priority, int debugFlag);
#endif
