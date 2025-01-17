
#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest03
*
* Spawns a simple child of a lower priority, then signals, and waits. 
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, kidpid;
    char* testName = "SchedulerTest03";
    char nameBuffer[512];

    /* spawn one simple child process at a lower priority. */
    console_output(FALSE, "\n%s: started\n", testName);
    
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);

    kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 1);

    console_output(FALSE, "%s: signaling first child\n", testName);
    k_kill(kidpid, SIG_TERM);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;

}
