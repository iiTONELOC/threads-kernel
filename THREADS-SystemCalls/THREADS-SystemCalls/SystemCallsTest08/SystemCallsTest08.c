#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "SystemCalls.h"
#include "TestCommon.h"

/*********************************************************************************
*
* SystemCallsTest00
*
*
*********************************************************************************/
int SystemCallsEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    int semaphore[MAXSEMS + 1];
    int sem_result;
    int i;

    /* Just output a message and exit. */
    console_output(FALSE, "\n%s: started\n", testName);

    console_output(FALSE, "%s: Creating MAXSEMS semaphores\n", testName);
    for (i = 0; i < MAXSEMS; i++) {
        sem_result = SemCreate(0, &semaphore[i]);
        if (sem_result == -1)
            console_output(FALSE, "%s: SemCreate returned %2d at index %3d\n", testName, sem_result, i);
    }

    sem_result = SemCreate(0, &semaphore[MAXSEMS]);

    if (sem_result != -1)
        console_output(FALSE, "%s: Error - SemCreate returned %2d instead of -1\n", testName, sem_result);

    console_output(FALSE, "%s: Freeing one semaphore\n", testName);
    sem_result = SemFree(semaphore[105]);

    if (sem_result != 0)
        console_output(FALSE, "%s: Error - SemFree returned %2d instead of 0\n", testName, sem_result);

    sem_result = SemCreate(0, &semaphore[MAXSEMS]);

    if (sem_result != -1)
    if (sem_result == 0)
        console_output(FALSE, "%s: TEST PASSED - SemCreate returned %2d\n", testName, sem_result);
    else {
        console_output(FALSE, "%s: Error - SemCreate returned %2d instead of 0\n", testName, sem_result);
    }

    Exit(8);

    return 0;
}
