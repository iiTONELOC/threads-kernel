#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

int SpawnAndUnblock(char* strArgs);

/*********************************************************************************
*
* SchedulerTest22
* 
* Tests block and unblock.
* 
* Expected Output:
* 
*********************************************************************************/
int SchedulerEntryPoint(void *pArgs)
{
    int status = -1, pid1, kidpid = -1;
    char nameBuffer[512];
    char* testName = "SchedulerTest22";

    console_output(FALSE, "\n%s: started\n", testName);
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    pid1 = k_spawn(nameBuffer, SpawnAndUnblock, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);

    /* Wait for the child and print the results. */
    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;
}

int SpawnAndUnblock(char* strArgs)
{
    int pids[BLOCK_UNBLOCK_COUNT], status = 0;
    char nameBuffer[512];

    console_output(FALSE, "%s: started\n", strArgs);

    for (int i = 0; i < 3; ++i)
    {
        console_output(FALSE, "%s: performing spawn of child\n", strArgs);
        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", strArgs, i + 1);
        pids[i] = k_spawn(nameBuffer, SimpleBockExit, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
        console_output(FALSE, "%s: after spawn of child with pid %d\n", strArgs, pids[i]);
    }

    display_process_table();

    for (int i = 0; i < 3; ++i)
    {
        console_output(FALSE, "%s: Unblocking process %d\n", strArgs, pids[i]);
        unblock(pids[i]);
    }

    for (int i = 0; i < 3; ++i)
    {
        int pid;
        console_output(FALSE, "%s: waiting for child process\n", strArgs);
        pid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs, pid, status);
    }

    k_exit(-2);

    return 0;
}