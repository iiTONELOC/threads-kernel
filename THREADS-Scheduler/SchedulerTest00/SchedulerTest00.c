
#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest00
*
* Simple test case that creates one child process, waits for the process to 
* terminate, and then exits.
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    char* testName = "SchedulerTest00";
    int status=-1, pid1, kidpid=-1;
    char nameBuffer[512];

    /* Spawn one simple child process at a lower priority. */
    console_output(FALSE, "\n%s: started\n", testName);

    /* Use the -Child naming convention for the child process name. */
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    pid1 = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);

    /* Wait for the child and print the results. */
    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;
}

