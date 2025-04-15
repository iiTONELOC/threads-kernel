#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "SystemCalls.h"
#include "TestCommon.h"
#include "libuser.h"

TestDiskParameters testCases[4][7];

/*********************************************************************************
*
* DevicesTest07
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
    int i, id, result;
    int trackPatterns[][7] = 
    { 
        {0,3,0,5,0,5,0 },
        {5,4,1,6,1,6,1 },
        {9,9,2,7,2,7,2 },
        {2,9,3,8,3,8,3 }
    };


    /* Initialize a set of 4 write/read patterns.  Each is given to a separate process. */
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            testCases[i][j].disk = 0;
            testCases[i][j].platter = 0;
            testCases[i][j].read = FALSE;
            testCases[i][j].sectorCount = 1;
            testCases[i][j].track = trackPatterns[i][j];
            testCases[i][j].sectorStart = trackPatterns[i][j] % 6;
        }
    }

    /* Just output a message and exit. */
    console_output(FALSE, "\t%s: started\n", testName);


    for (i = 0; i < 4; i++)
    {
        /* 0 sleep time ... */
        optionSeparator = CreateDevicesTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, 0, testCases[i], 7, 0);
        Spawn(nameBuffer, DevicesTestDriver, nameBuffer, THREADS_MIN_STACK_SIZE, 3, &kidPid);
        optionSeparator[0] = '\0';
    }

    /* no output to see the pattern */
    for (i = 0; i < 4; i++)
    {
        result = Wait(&kidPid, &id);
    }

    console_output(FALSE, "%s:\tTest Complete\n", testName);

    Exit(0);

    return 0;
}

