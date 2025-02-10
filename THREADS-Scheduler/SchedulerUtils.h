#pragma once
#ifndef SCHEDULER_UTILS_H
#define SCHEDULER_UTILS_H
#include <stdlib.h>
#include "PriorityProcessQueue.h"

#define PROCESS_TABLE_ROW_FORMAT "%-7d %-8d %-9d %-13s %-8d %-8llu %s\n"
#define PROCESS_TABLE_HEADER_FORMAT "%-7s %-8s %-9s %-13s %-8s %-8s %-8s\n"

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

/**
 * @brief Validate the k_spawn parameters
 *
 * @param name The name of the process
 * @param entryPoint The entry point of the process
 * @param arg The arguments to pass to the process
 * @param stacksize The size of the stack
 * @param priority The priority of the process
 * @param debugFlag The debug flag
 *
 * @return int The return value of the function
 */
int ValidateKSpawnParams(char *name, int (*entryPoint)(void *), void *arg, int stacksize,
                         int priority, int debugFlag);

/**
 * @brief Displays a row in the process table
 *
 * @param pProcess Pointer to the process to display
 */
void PrintProcessRow(Process *pProcess);

/**
 * @brief Print the process table
 *
 * @param usingTablePtr The process table to print
 * @param size The size of the process table
 * @param currentNumProcesses The current number of processes
 */
void PrintProcessTable(Process *usingTablePtr, int size, int currentNumProcesses);

#endif
