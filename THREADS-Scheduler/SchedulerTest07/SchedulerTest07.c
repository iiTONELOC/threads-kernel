#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest07
*
* Spawns a lower priority process that spawns a higher priority process. 
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, kidpid;
    char nameBuffer[512];
    char* testName = "SchedulerTest07";


    console_output(FALSE, "\n%s: started\n", testName);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    kidpid = k_spawn(nameBuffer, SpawnOnePriorityFour, nameBuffer, THREADS_MIN_STACK_SIZE, 2);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;

} 