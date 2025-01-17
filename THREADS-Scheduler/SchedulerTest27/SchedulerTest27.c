#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

int SpawnTwoDifferentPriorities(char* strArgs);
int SpawnJoiner(char* strArgs);
int pidToJoin;

/*********************************************************************************
*
* SchedulerTest27
*
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, kidpid;
    char nameBuffer[512];
    char* testName = "SchedulerTest27";

    console_output(FALSE, "\n%s: started\n", testName);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    kidpid = k_spawn(nameBuffer, SpawnOnePriorityFour, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, kidpid);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child2", testName);
    kidpid = k_spawn(nameBuffer, SpawnJoiner, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, kidpid);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child3", testName);
    pidToJoin = k_spawn(nameBuffer, SpawnTwoDifferentPriorities, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
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

int SpawnTwoDifferentPriorities(char* strArgs)
{
    int kidpid;
    int status = 0xff;
    char nameBuffer[1024];

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);
        console_output(FALSE, "%s: performing spawn of first child\n", strArgs);

        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", strArgs);
        kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
        console_output(FALSE, "%s: spawn of first child returned pid = %d\n", strArgs, kidpid);

        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child2", strArgs);
        console_output(FALSE, "%s: performing spawn of second child\n", strArgs);
        kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 4);
        console_output(FALSE, "%s: spawn of second child returned pid = %d\n", strArgs, kidpid);

        console_output(FALSE, "%s: performing first wait\n", strArgs);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs,
            kidpid, status);

        console_output(FALSE, "%s: performing second wait\n", strArgs);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs,
            kidpid, status);
    }
    k_exit(-3);

    return 0;
}
