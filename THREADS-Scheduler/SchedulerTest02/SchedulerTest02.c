
#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest02
*
* Spawns one child process at priority 3.  The child process spawns two
* children at a higher priority (4).  Those children should preempt their parent
* at spawn().
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, pid1, kidpid = -1;
    char* testName = "SchedulerTest02";
    char nameBuffer[512];

    console_output(FALSE, "\n%s: started\n", testName);

    /* Use the -Child naming convention for the child process name. */
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);

    pid1 = k_spawn(nameBuffer, SpawnTwoPriorityFour, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);

    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    return 0;
}

