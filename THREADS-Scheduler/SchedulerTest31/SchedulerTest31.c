
#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

/*********************************************************************************
*
* SchedulerTest31
*
* Test block with an out of range value.
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    char* testName = "SchedulerTest31";

    console_output(FALSE, "\n%s: started\n", testName);

    /* Use the -Child naming convention for the child process name. */
    console_output(FALSE, "%s: blocking with a value of 6\n", testName);
    block(6);

    k_exit(0);

    return 0;
}

