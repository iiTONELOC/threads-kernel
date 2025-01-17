#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

int JoinSelfTest(char* strArgs); 

/*********************************************************************************
*
* SchedulerTest20
* 
* Tests a process that tries to join itself
* 
* Expected Output:
* 
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, pid1, kidpid = -1;
    char nameBuffer[512];
    char* testName = "SchedulerTest20";

    console_output(FALSE, "\n%s: started\n", testName);
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    pid1 = k_spawn(nameBuffer, JoinSelfTest, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);

    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    return 0;
}

int JoinSelfTest(char* strArgs)
{
    int pid, status=0, result;

    console_output(FALSE, "%s: started\n", strArgs);

    pid = k_getpid();

    console_output(FALSE, "%s: joining with pid %d (should fail)\n", strArgs, pid);
    result = k_join(pid, &status);
    console_output(FALSE, "%s: k_join returned %d\n", strArgs, result);

    k_exit(-2);

    return 0;
}
