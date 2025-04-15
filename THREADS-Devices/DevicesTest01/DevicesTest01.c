#define _CRT_SECURE_NO_WARNINGS
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
* DevicesTest01
*
* Spawns 10 process that each sleep different lengths of time
* 
*********************************************************************************/
int DevicesEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    char nameBuffer[512];
    int childId = 0;
    int kidPid;
    char* optionSeparator;
    int messageCount = 0;
    int sleepTime = 1;
    char childNames[MAXPROC][256];
    int i, id, result;


    /* Just output a message and exit. */
    console_output(FALSE, "\n%s: started\n", testName);

    for (i = 0; i < 10; i++) 
    {
        /* set sleep time */
        optionSeparator = CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, sleepTime, 0, 0, 0);
        Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 3, &kidPid);
        optionSeparator[0] = '\0';
        strncpy(childNames[kidPid], nameBuffer, 256);

        sleepTime += 2;
    }


    for (i = 0; i < 10; i++) 
    {
        console_output(FALSE, "%s: Waiting on Child\n", testName);
        result = Wait(&kidPid, &id);
        console_output(FALSE, "%s: Wait returned %d, pid:%d, status %d\n", testName, result, kidPid, id);
    }

    Exit(0);

    return 0;
}
