#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest12
*
* Tests the exit function kernel mode validation.
*
* Expected Output:
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, pid1, kidpid = -1;
    char nameBuffer[512];
    char* testName = "SchedulerTest12";

    console_output(FALSE, "\n%s: started\n", testName);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);

    pid1 = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, pid1);

    /* Wait for the child and print the results. */
    console_output(FALSE, "%s: waiting for child process\n", testName);
    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    /* set the mode to user mode. */
    set_psr(get_psr() & ~PSR_KERNEL_MODE);

    k_exit(0);
    return 0; /* The compiler wants to see this... */
}
