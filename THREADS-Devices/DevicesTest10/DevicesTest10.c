#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "SystemCalls.h"
#include "TestCommon.h"
#include "libuser.h"

TestDiskParameters testCases[50];

/*********************************************************************************
*
* DevicesTest10
*
* Disk track pattern testing.
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
    char childNames[MAXPROC][256];
    int i, id, result;
    int trackPatterns[] = { 5,3,9,0,7,2,1,6 };

    /* Just output a message and exit. */
    console_output(FALSE, "\n%s: started\n", testName);

    /* Scheduling test with one track, different sectors. */
    for (int i = 0; i < 8; ++i)
    {
        testCases[i].disk = 0;
        testCases[i].platter = 0;
        testCases[i].read = FALSE;
        testCases[i].sectorCount = 1;
        testCases[i].track = 3;
        testCases[i].sectorStart = trackPatterns[i];

        /* 0 sleep time ... */
        optionSeparator = CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, 0, &testCases[i], 1, 0);
        Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 2, &kidPid);
        optionSeparator[0] = '\0';
        strncpy(childNames[kidPid], nameBuffer, 256);
    }



    /* wait with no output to show the results. */
    for (i = 0; i < 8; i++)
    {
        result = Wait(&kidPid, &id);
    }

    console_output(FALSE, "%s: Test complete\n", testName, result, kidPid, id);
    Exit(0);

    return 0;
}
