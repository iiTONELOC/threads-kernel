#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest19
*
* Fill process table and verify the return value of -1
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* args)
{
    int status = -1, kidpid = -1;
    char* testName = "SchedulerTest19";
    char nameBuffer[512];

    console_output(FALSE, "\n%s: started\n", testName);

    for (int i = 0; i < MAXPROC+2; ++i)
    {
        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", testName, i + 1);
        kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
        if (kidpid == -1)
        {
            console_output(FALSE, "%s: k_spawn returned -1 at attempt %d\n", testName, i+1);
        }
    }
    for (int i = 0; i < MAXPROC; ++i)
    {
        console_output(FALSE, "%s: waiting for child process\n", testName);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);
    }

    k_exit(0);

    return 0;
}