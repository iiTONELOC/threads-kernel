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
* DevicesTest12
*
* Mix of sleeps and disk operations.
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

    /* Just output a message and exit. */
    console_output(FALSE, "\n%s: started\n", testName);


    /* Spawn 5 processes to sleep. */
    for (i = 0; i < 5; ++i)
    {

        /* i sleep time ... */
        optionSeparator = CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, i, 0, 0, 0);
        Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 2, &kidPid);
        optionSeparator[0] = '\0';
        strncpy(childNames[kidPid], nameBuffer, 256);
    }

    testCases[0].disk = 0;
    testCases[0].platter = 0;
    testCases[0].read = 0;
    testCases[0].sectorCount = 1;
    testCases[0].track = 7;
    testCases[0].sectorStart = 9;

    /* Write message A to disk 0 */
    optionSeparator = CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, 0, &testCases[0], 1, DEVICES_OPTION_MESSAGEA);
    Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 2, &kidPid);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidPid], nameBuffer, 256);

    /* Write message B to disk 0 */
    optionSeparator = CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, 0, &testCases[0], 1, DEVICES_OPTION_MESSAGEB);
    Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 2, &kidPid);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidPid], nameBuffer, 256);

    testCases[1].disk = 1;
    testCases[1].platter = 0;
    testCases[1].read = 1;
    testCases[1].sectorCount = 1;
    testCases[1].track = 7;
    testCases[1].sectorStart = 9;

    /* Read message A from disk 1 */
    optionSeparator = CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, 0, &testCases[1], 1, DEVICES_OPTION_MESSAGEA);
    Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 1, &kidPid);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidPid], nameBuffer, 256);

    /* Read message B from disk 1 */
    optionSeparator = CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, 0, &testCases[1], 1, DEVICES_OPTION_MESSAGEB);
    Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 1, &kidPid);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidPid], nameBuffer, 256);

    /* wait with no output to show the results. */
    for (i = 0; i < 9; i++)
    {
        result = Wait(&kidPid, &id);
    }
    console_output(FALSE, "%s: Test complete\n", testName, result, kidPid, id);
    Exit(0);

    return 0;
}
