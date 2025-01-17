#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

int SpawnTwoPriorities(char* strArgs);
int UnblockTwoPriorities(char* strArgs);
extern int pids[];

/*********************************************************************************
*
* SchedulerTest23
*
* Tests block and unblock.
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, pid1, kidpid = -1;
    char nameBuffer[512];
    char* testName = "SchedulerTest23";

    console_output(FALSE, "\n%s: started\n", testName);
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    pid1 = k_spawn(nameBuffer, SpawnTwoPriorities, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);

    /* Wait for the child and print the results. */
    console_output(FALSE, "%s: joining child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;
}


int SpawnTwoPriorities(char* strArgs)
{
    int kidpid, status = 0;
    char nameBuffer[512];

    console_output(FALSE, "%s: started\n", strArgs);

    for (int i = 0; i < BLOCK_UNBLOCK_COUNT; ++i)
    {
        console_output(FALSE, "%s: performing spawn of child\n", strArgs);
        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", strArgs, i + 1);
        pids[i] = k_spawn(nameBuffer, SimpleBockExit, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
        console_output(FALSE, "%s: after spawn of child with pid %d\n", strArgs, pids[i]);
    }

    for (int i = BLOCK_UNBLOCK_COUNT; i < BLOCK_UNBLOCK_COUNT*2; ++i)
    {
        console_output(FALSE, "%s: performing spawn of child\n", strArgs);
        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", strArgs, i + 1);
        pids[i] = k_spawn(nameBuffer, SimpleBockExit, nameBuffer, THREADS_MIN_STACK_SIZE, 4);
        console_output(FALSE, "%s: after spawn of child with pid %d\n", strArgs, pids[i]);
    }

    display_process_table();

    console_output(FALSE, "%s: performing spawn of child\n", strArgs);
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", strArgs, BLOCK_UNBLOCK_COUNT * 2);
    kidpid = k_spawn(nameBuffer, UnblockTwoPriorities, nameBuffer, THREADS_MIN_STACK_SIZE, 5);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", strArgs, kidpid);

    console_output(FALSE, "%s: waiting for child process\n", strArgs);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs, kidpid, status);

    for (int i = 0; i < BLOCK_UNBLOCK_COUNT * 2; ++i)
    {
        console_output(FALSE, "%s: waiting for child process\n", strArgs);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs, kidpid, status);
    }

    k_exit(-2);

    return 0;
}

