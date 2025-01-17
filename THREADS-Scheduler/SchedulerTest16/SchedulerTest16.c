#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

#define PID_TEST_COUNT 5
int childPids[PID_TEST_COUNT];
int VerifyGetPid(char* strArgs);
/*********************************************************************************
*
* SchedulerTest16
*
* Verify k_getpid()
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* args)
{
    int status = -1, kidpid = -1;
    char* testName = "SchedulerTest16";
    char nameBuffer[512];

    console_output(FALSE, "\n%s: started\n", testName);

    for (int i = 0; i < PID_TEST_COUNT; ++i)
    {
        console_output(FALSE, "%s: performing spawn of child\n", testName);
        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", testName, i+1);
        childPids[i] = k_spawn(nameBuffer, VerifyGetPid, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
        console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, childPids[i]);
    }

    for (int i = 0; i < PID_TEST_COUNT; ++i)
    {
        console_output(FALSE, "%s: waiting for child process\n", testName);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);
    }

    k_exit(0);

    return 0;
}

int VerifyGetPid(char* strArgs)
{
    static int i = 0;
    int pid;
    if ((pid = k_getpid()) != childPids[i])
    {
        console_output(FALSE, "%s: k_getPid test FAILED by returning %d\n", strArgs, pid);
    }
    else
    {
        console_output(FALSE, "%s: k_getPid test PASSED by returning %d\n", strArgs, pid);

    }
    ++i;
    k_exit(-2);

    return 0;
}