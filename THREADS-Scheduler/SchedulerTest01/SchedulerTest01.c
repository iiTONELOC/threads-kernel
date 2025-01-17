
#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest01
*
* Creates a child process of a lower priority (3) and waits for it to exit.
* The child process creates two child processes of a lower priority (2) and 
* waits for them to exit. The SchedulerTest01 process does not itself call k_exit().
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, pid1, kidpid = -1;
    char* testName = "SchedulerTest01";
    char nameBuffer[512];

    console_output(FALSE, "\n%s: started\n", testName);

    /* Use the -Child naming convention for the child process name. */
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);

    pid1 = k_spawn(nameBuffer, SpawnTwoPriorityTwo, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);

    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    return 0;
}
