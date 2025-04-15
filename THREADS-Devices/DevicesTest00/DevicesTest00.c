#include <stdio.h>
#include <stdlib.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "SystemCalls.h"
#include "TestCommon.h"
#include "libuser.h"

/*********************************************************************************
*
* DevicesTest00
* 
* Single process that sleeps for 10 seconds and measures the sleep time.
*
*********************************************************************************/
int DevicesEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    int begin, end;
    int timeInMilliseconds;

    /* Just output a message and exit. */
    console_output(FALSE, "\n%s: Started\n", testName);

    console_output(FALSE, "%s:  Going to sleep for 10 seconds\n", testName);

    GetTimeofDay(&begin);
    SleepSeconds(10);
    GetTimeofDay(&end);
    timeInMilliseconds = (end - begin) / 1000;

    console_output(FALSE, "%s: Slept %d\n", testName, timeInMilliseconds);

    Exit(0);

    return 0;
}
