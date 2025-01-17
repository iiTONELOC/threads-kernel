#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

extern int SpawnDumpProcess(char* strArgs);
extern int SignalJoinGlobalPid(char* arg);
extern int gPid;


/*********************************************************************************
*
* SchedulerTest14
*
* This test verifies the status of blocked on join and blocked on wait
* and tests the correct statuses and return codes for a signaled process.
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, pid2, kidpid = -1;
    char* testName = "SchedulerTest14";
    char nameBuffer[512];

    console_output(FALSE, "\n%s: started\n", testName);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    gPid = k_spawn(nameBuffer, SpawnDumpProcess, nameBuffer, THREADS_MIN_STACK_SIZE, 4);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, gPid);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child2", testName);
    pid2 = k_spawn(nameBuffer, SignalJoinGlobalPid, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid2);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);

    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);
    return 0;
}

