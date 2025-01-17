#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

int SpawnTwoDifferentPriorities(char* strArgs);
int SpawnJoiner(char* strArgs);
extern int pidToJoin;

/*********************************************************************************
*
* SchedulerTest26
*
*   Tests join by trying to join with a process that has already exited.
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, kidpid;
    char nameBuffer[512];
    char* testName = "SchedulerTest26";

    console_output(FALSE, "\n%s: started\n", testName);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    pidToJoin = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 4);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pidToJoin);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child2", testName);
    kidpid = k_spawn(nameBuffer, SpawnJoiner, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, kidpid);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;
}
