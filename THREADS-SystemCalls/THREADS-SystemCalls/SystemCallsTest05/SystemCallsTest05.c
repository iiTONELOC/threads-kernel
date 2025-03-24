#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "SystemCalls.h"
#include "TestCommon.h"
#include "libuser.h"

/*********************************************************************************
*
* SystemCallsTest05
* 
* Tests the handling of creating more than MAXSEMS semaphores.
*
*********************************************************************************/
int SystemCallsEntryPoint(void* pArgs)
{
    int semaphore;
    int sem_result;
    int i;
    char* testName = GetTestName(__FILE__);

    /* Just output a message and exit. */
    console_output(FALSE, "\n%s: started\n", testName);

    for (i = 0; i < MAXSEMS + 2; i++) {
        sem_result = SemCreate(0, &semaphore);
        console_output(FALSE, "%s: SemCreate returned %2d at index %3d\n", testName, sem_result, i);
    }
    Exit(8);

    return 0;
}
