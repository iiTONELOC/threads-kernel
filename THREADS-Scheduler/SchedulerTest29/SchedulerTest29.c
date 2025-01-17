#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"


int RunUntilSignaled(char* strArgs);

/*********************************************************************************
*
* SchedulerTest29
*
* Test verifies the time slicing functions by starting a process that 
* runs until signaled and then goes through the following
* logic:
*    spawn four children
*    each child delays in a busy wait and then exits
*    the child with a name that is divisible by 4 dumps the process table
*
* A loop of this logic is repeated 3 times.The proc table should show non-zero values for
* CPU time for each of the children since their delays are larger than the timeslice.
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, kidpid = -1, longRunningPid;
    int i, j;
    char nameBuffer[1028];
    char* testName = "SchedulerTest29";
    int count = 1;

    console_output(FALSE, "\n%s: started\n", testName);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", testName, count++);
    longRunningPid = k_spawn(nameBuffer, RunUntilSignaled, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, longRunningPid);

    for (j = 0; j < 3; j++)
    {
        for (i = 2; i < 6; i++)
        {
            snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", testName, count++);
            kidpid = k_spawn(nameBuffer, DelayAndDump, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
            console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, kidpid);
        }

        for (i = 2; i < 6; i++)
        {
            kidpid = k_wait(&status);
            console_output(FALSE, "%s: exit status for child %d is %d\n",
                testName, kidpid, status);
        }
    }

    /* terminate and wait for the first process.*/
    k_kill(longRunningPid, SIG_TERM);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n",
        testName, kidpid, status);

    k_exit(0);

    return 0;
}


int RunUntilSignaled(char* strArgs)
{

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);
        console_output(FALSE, "%s: performing spawn of first child\n", strArgs);

        while (!signaled());
    }
    k_exit(-3);

    return 0;
}