#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest18
* 
* Verify stack size checks.
* 
* Expected Output:
* 
*********************************************************************************/
int SchedulerEntryPoint(void *pArgs)
{
    int status=-1, pid1, kidpid=-1;
    char nameBuffer[512];
    char* testName = "SchedulerTest18";

    console_output(FALSE, "\n%s: started\n", testName);
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    pid1 = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE -10 , 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);
    if (pid1 == -2)
    {
        console_output(FALSE, "%s: TEST PASSED\n", testName);
    }
    else
    {
        console_output(FALSE, "%s: TEST FAILED\n", testName);

        /* Wait for the child and print the results. */
        console_output(FALSE, "%s: joining child process\n", testName);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    }
    k_exit(0);

    return 0;
}
