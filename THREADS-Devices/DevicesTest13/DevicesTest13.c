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
* DevicesTest13
*
*
*********************************************************************************/
int DevicesEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    char nameBuffer[512];
    int childId = 0;
    int kidPid;
    int status;
    char* optionSeparator;
    int messageCount = 0;
    int i, id, result;

    /* Just output a message and exit. */
    console_output(FALSE, "\n%s: started\n", testName);

    for (i = 0; i < 15; ++i)
    {
        testCases[i].disk = 0;
        testCases[i].platter = 0;
        testCases[i].read = 0;
        testCases[i].sectorCount = 1;
        testCases[i].track = 6;
        testCases[i].sectorStart = 9;
    }

    /* Sets the starting track to 6 */
    optionSeparator = CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, 0, &testCases[0], 1, DEVICES_OPTION_MESSAGEA);
    Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 2, &kidPid);
    Wait(&kidPid, &status);

    testCases[1].track = 6;
    testCases[2].track = 15;
    testCases[3].track = 3;
    testCases[4].track = 4;
    testCases[5].track = 7;
    testCases[6].track = 11;
    testCases[7].track = 12;

    for (i = 1; i < 8; ++i)
    {
        CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, 0, &testCases[i], 1, DEVICES_OPTION_MESSAGEB);
        Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 5, &kidPid);
    }

    /* wait with no output to show the results. */
    for (i = 0; i < 7; i++)
    {
        result = Wait(&kidPid, &id);
    }

    testCases[8].track = 3;
    testCases[9].track = 14;
    testCases[10].track = 4;
    testCases[11].track = 1;
    testCases[12].track = 6;
    testCases[13].track = 5;
    testCases[14].track = 9;

    for (i = 8; i < 15; ++i)
    {
        CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, 0, &testCases[i], 1, DEVICES_OPTION_MESSAGEB);
        Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 5, &kidPid);
    }

    /* wait with no output to show the results. */
    for (i = 0; i < 7; i++)
    {
        result = Wait(&kidPid, &id);
    }

    console_output(FALSE, "%s: Test complete\n", testName, result, kidPid, id);
    Exit(0);

    return 0;
}
