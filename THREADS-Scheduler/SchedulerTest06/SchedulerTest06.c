#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

int SimpleDelayExit(void* pArgs);
void SystemDelay(int millisTime);
int SpawnTwoPriorityFour(char* pArgs);
int DelayAndDump(char* arg);
int SignalAndJoinTwoLower(char* strArgs);


/*********************************************************************************
*
* SchedulerTest06
*
* Tests basic signaling features.
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, pid1, kidpid = -1;
    char* testName = "SchedulerTest06";
    char nameBuffer[512];

    /* spawn one simple child process at a lower priority. */
    console_output(FALSE, "\n%s: started\n", testName);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    pid1 = k_spawn(nameBuffer, SignalAndJoinTwoLower, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);
 
    display_process_table();

    /* Wait for the child and print the results. */
    console_output(FALSE, "%s: joining child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;
}
