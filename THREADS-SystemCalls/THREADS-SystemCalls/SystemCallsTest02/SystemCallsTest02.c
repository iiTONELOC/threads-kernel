#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "SystemCalls.h"
#include "TestCommon.h"

/*********************************************************************************
*
* SystemCallsTest02
*
* Testing simple termination of the system when the user process returns.
* 
*********************************************************************************/
int SystemCallsEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);

    /* Output a message and return.  This should result in a proper exit. */
    console_output(FALSE, "\n%s: started and returning -1.\n", testName);

    return -1;
}
