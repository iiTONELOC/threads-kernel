#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest05
*
* Test verifies the time slicing functions by going through the following 
* logic:
*    spawn four children 
*    each child delays in a busy wait and then exits
*    child with a name that ends in a number divisible by 4 dumps the process table
*
* A loop of this logic is repeated 3 times.The proc table should show non-zero values 
* for CPU time for each of the children since their delays are larger than the 
* timeslice.
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, kidpid = -1;
    int i, j;
    char nameBuffer[1028];
    char* testName = "SchedulerTest05";
    int count = 1;

    console_output(FALSE, "\n%s: started\n", testName);
    for (j = 0; j < 3; j++)
    {
        for (i = 2; i < 6; i++)
        {
            snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", testName, count++);
            kidpid = k_spawn(nameBuffer, DelayAndDump, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
            console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, kidpid);
        }

        for (i = 2; i < 6; i++)
        {
            kidpid = k_wait(&status);
            console_output(FALSE, "%s: exit status for child %d is %d\n",
                testName, kidpid, status);
        }
    }
    k_exit(0);

    return 0;
}
