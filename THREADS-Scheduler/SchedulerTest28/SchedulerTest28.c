#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

int SpawnTwoDifferentPriorities(char* strArgs);
extern int SpawnJoiner(char* strArgs);  // from test27
extern int pidToJoin;

/*********************************************************************************
*
* SchedulerTest28
*
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, kidpid;
    char nameBuffer[512];
    char* testName = "SchedulerTest28";

    console_output(FALSE, "\n%s: started\n", testName);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 4);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, kidpid);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child2", testName);
    kidpid = k_spawn(nameBuffer, SpawnJoiner, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, kidpid);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child3", testName);
    pidToJoin = k_spawn(nameBuffer, SpawnOnePriorityOne, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pidToJoin);


    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;
}
