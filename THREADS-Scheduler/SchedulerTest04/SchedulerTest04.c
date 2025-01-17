
#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest04
*
* Spawns max number of processes, dumps the proc table, waits for all, then repeats
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, kidpid = -1;
    int i, j;
    char nameBuffer[512];
    char* testName = "SchedulerTest04";
    int count = 1;

    console_output(FALSE, "\n%s: started\n", testName);
    for (j = 0; j < 2; j++)
    {
        for (i = 2; i < MAX_PROCESSES; i++)
        {
            snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", testName, count++);
            kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
            console_output(FALSE, "%s: after spawn of child %d\n", testName, kidpid);
        }

        display_process_table();

        for (i = 2; i < MAX_PROCESSES; i++)
        {
            kidpid = k_wait(&status);
            console_output(FALSE, "%s: exit status for child %d is %d\n",
                testName, kidpid, status);
        }
    }

    fflush(stdout);

    k_exit(0);

    return 0;
}


