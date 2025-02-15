#pragma once
#ifndef SCHEDULER_UTILS_H
#define SCHEDULER_UTILS_H
#include <stdlib.h>
#include "../Include/THREADSLib.h"

#ifndef NULL
#define NULL (void *)0
#endif

#ifndef MAXNAME
#define MAXNAME 256
#endif

#ifndef MAXARG
#define MAXARG 256
#endif

#ifndef LOWEST_PRIORITY
#define LOWEST_PRIORITY 0
#endif

#ifndef HIGHEST_PRIORITY
#define HIGHEST_PRIORITY 5
#endif

#ifndef THREADS_MIN_STACK_SIZE
#define THREADS_MIN_STACK_SIZE 8192
#endif

/*_______________________Function Prototypes_______________________*/

void TrimRight(char *pString);

void CopyString(char *pSource, char *pDestination, size_t size);

int ValidateKSpawnParams(char *name, int (*entryPoint)(void *), void *arg, int stacksize,
                         int priority, int debugFlag);

#endif
