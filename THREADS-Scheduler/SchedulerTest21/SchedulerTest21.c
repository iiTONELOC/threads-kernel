#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest21
* 
* Verifys priority bounds.
* 
* Expected Output:
* 
*********************************************************************************/
int SchedulerEntryPoint(void *pArgs)
{
    int status = -1, kidpid = 0;
    char nameBuffer[512];
    char* testName = "SchedulerTest21";

    console_output(FALSE, "\n%s: started\n", testName);
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, -10);
    if (kidpid == -1)
    {
        console_output(FALSE, "%s: Could not spawn process, invalid priority\n", testName);
    }

    kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 7);
    if (kidpid == -1)
    {
        console_output(FALSE, "%s: Could not spawn process, invalid priority\n", testName);
    }

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    return 0;
}
