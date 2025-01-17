#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

#define JOIN_TEST_COUNT 10

extern int JoinProcess(char* strArgs); // from test 15


/*********************************************************************************
*
* SchedulerTest17
*
* Spawns one lower priority process then JOIN_TEST_COUNT processes that join
* with the lower priority process.
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, kidpid = -1;
    char* testName = "SchedulerTest17";
    char nameBuffer[512];

    console_output(FALSE, "\n%s: started\n", testName);

    console_output(FALSE, "%s: performing spawn of child\n", testName);
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    gPid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 2);

    for (int i = 0; i < JOIN_TEST_COUNT; ++i)
    {
        console_output(FALSE, "%s: performing spawn of child\n", testName);
        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", testName, i + 2);
        kidpid = k_spawn(nameBuffer, JoinProcess, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
        console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, kidpid);
    }

    for (int i = 0; i < JOIN_TEST_COUNT+1; ++i)
    {
        console_output(FALSE, "%s: waiting for child process\n", testName);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);
    }

    k_exit(0);

    return 0;

}
