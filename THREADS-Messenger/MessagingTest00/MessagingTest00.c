#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"

/*********************************************************************************
*
* MessagingTest00
*
* Simple test case that simply exits.
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);

    /* Just output a message and exit. */
    console_output(FALSE, "\n%s: started\n", testName);

    k_exit(0);

    return 0;
}
