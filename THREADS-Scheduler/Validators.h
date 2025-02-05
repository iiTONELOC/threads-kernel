#pragma once

#ifndef SCHEDULER_VALIDATORS_H
#define SCHEDULER_VALIDATORS_H

// __________________________ Function Prototypes __________________________

/**
 * @brief Validate params for the k_spawn function
 *
 * @param name A pointer to the name of the process
 * @param entryPoint A pointer to the entry point of the process
 * @param arg A pointer to the arguments of the process
 * @param stacksize The size of the stack
 * @param priority The priority of the process
 * @param debugFlag The debug flag
 */
int ValidateKSpawnParams(char *name, int (*entryPoint)(void *), void *arg,
                         int stacksize, int priority, int debugFlag);
#endif
