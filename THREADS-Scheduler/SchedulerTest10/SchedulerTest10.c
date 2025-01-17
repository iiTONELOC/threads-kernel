#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

int parentPid = -1;

int JoinParent(char* args);

/*********************************************************************************
*
* SchedulerTest10
* 
* Spawns a process that attempts to join with it's parent
* 
* Expected Output:
* 
*********************************************************************************/
int SchedulerEntryPoint(void *pArgs)
{
    int status=-1, pid1, kidpid=-1;
    char nameBuffer[512];
    char* testName = "SchedulerTest10";

    /* spawn one simple child process at a lower priority. */
    console_output(FALSE, "\n%s: started\n", testName);

    parentPid = k_getpid();

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);

    pid1 = k_spawn(nameBuffer, JoinParent, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);

    /* Wait for the child and print the results. */
    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;
}

int JoinParent(char* strArgs)
{
    int exitCode;

    console_output(FALSE, "%s: started\n", strArgs);
    console_output(FALSE, "%s: joining parent process\n", strArgs);
    k_join(parentPid, &exitCode);
    console_output(FALSE, "%s: finished joining parent process\n", strArgs);

    k_exit(-3);

    return 0;

}