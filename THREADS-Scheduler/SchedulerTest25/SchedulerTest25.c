#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest08
* 
* Simple test case that creates one child process.
* 
* Expected Output:
* 
*********************************************************************************/
int SchedulerEntryPoint(void *pArgs)
{
    int status=-1, pid1, kidpid=-1;
    char* testName = "SchedulerTest25";

    /* spawn one simple child process at a lower priority. */
    console_output(FALSE, "\n%s: started\n", testName);
    pid1 = k_spawn("P1-Child1", SimpleDelayExit, "P1-Child1", THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);

    /* Wait for the child and print the results. */
    console_output(FALSE, "%s: joining child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;
}
