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
* DevicesTest11
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
    char childNames[512][256];
    int i, id, result;
    int trackPatterns[] = { 11,12,4,5,0,9,4,12,9,2,10,0,9,13,14,13 };

    /* Just output a message and exit. */
    console_output(FALSE, "\n%s: started\n", testName);

    /* Mix of reads and writes. */
    for (int i = 0; i < 16; ++i)
    {
        testCases[i].disk = 0;
        testCases[i].platter = 0;
        testCases[i].read = i % 2;
        testCases[i].sectorCount = 1;
        testCases[i].track = trackPatterns[i % 16];
        testCases[i].sectorStart = trackPatterns[i % 16] % 4;

        /* 0 sleep time ... */
        optionSeparator = CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, 0, &testCases[i], 1, 0);
        Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 2, &kidPid);
        optionSeparator[0] = '\0';
        strncpy(childNames[kidPid], nameBuffer, 256);
    }



    /* wait with no output to show the results. */
    for (i = 0; i < 16; i++)
    {
        result = Wait(&kidPid, &id);
    }
    console_output(FALSE, "%s: Test complete\n", testName, result, kidPid, id);
    Exit(0);

    return 0;
}
