#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "THREADSLib.h"
#include "TestCommon.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "libuser.h"

static testNameBuffer[512];

/* Can only get this for the test name since there is one buffer for this. */
char* GetTestName(char* filename)
{
    char* testName;

    testName = filename;
    if (strrchr(filename, '\\'))
    {
        testName = strrchr(filename, '\\') + 1;
    }

    strncpy((char*)testNameBuffer, testName, strlen(testName) - 2);

    return (char*)testNameBuffer;
}

char* CreateDevicesTestArgs(char* buffer, int bufferSize, char* prefix, int childId, int sleepTime, TestDiskParameters* testList, int testListCount, unsigned char options)
{
    char* separator;

    snprintf(buffer, bufferSize, "%s-Child%d:%d-%p-%d-%d", prefix, childId, sleepTime, testList, testListCount, options);

    separator = strchr(buffer, ':');

    return separator;
}


int DevicesTestDriver(char* strArgs)
{
    char* separator;
    int sendCount = 0;
    int receiveCount = 0;
    int options = 0;
    int bSending = 0;
    int sleepSeconds;
    int messageStartNumber = 0;
    int blocking = TRUE;
    int begin, end;
    int timeInMilliseconds;
    TestDiskParameters *testCases;
    int testCaseCount;
    char messageA[] = "The cake is a lie. Disk 0 writes truth.";
    char messageB[] = "Interrupts don't scare me. Disk 1 ready.";


    /* parse the args */
    separator = strchr(strArgs, ':');
    separator[0] = '\0';
    separator++;
    sscanf(separator, "%d-%p-%d-%d", &sleepSeconds, &testCases, &testCaseCount, &options);

    console_output(FALSE, "%s:\tstarted\n", strArgs);

    if (sleepSeconds > 0)
    {
        console_output(FALSE, "%s:\tSleeping for %d seconds\n", strArgs, sleepSeconds);
        GetTimeofDay(&begin);
        SleepSeconds(sleepSeconds);
        GetTimeofDay(&end);
        timeInMilliseconds = (end - begin) / 1000;

        console_output(FALSE, "%s:\tSlept %d milliseconds\n", strArgs, timeInMilliseconds);
    }

    for (int i = 0; i < testCaseCount; ++i)
    {
        unsigned char *dataBuffer;
        char device[16];
        int status;

        sprintf(device, "disk%d", testCases->disk);
        dataBuffer = malloc(THREADS_DISK_SECTOR_SIZE * testCases->sectorCount);
        if (testCases->read)
        {
            memset(dataBuffer, 0, THREADS_DISK_SECTOR_SIZE * testCases->sectorCount);
            DiskRead(device, dataBuffer, testCases->platter, testCases->track, testCases->sectorStart, testCases->sectorCount, &status);
            console_output(FALSE, "%s:\tDiskRead  (%s) -  platter %d, track %02d, sectorStart %02d, sectorCount %02d, status 0x%08x\n",
                strArgs, device, testCases->platter, testCases->track, testCases->sectorStart, testCases->sectorCount, status);

            if (options != 0)
            {
                if (options & DEVICES_OPTION_MESSAGEA)
                {
                    if (memcmp(dataBuffer, messageA, strlen(messageA) + 1))
                    {
                        console_output(FALSE, "%s:\tDiskRead  (%s) -  Output match!\n", strArgs, device);
                    }
                    else
                    {
                        console_output(FALSE, "%s:\tDiskRead  (%s) -  Output mis-match!\n", strArgs, device);
                    }
                }
                else if (options & DEVICES_OPTION_MESSAGEB)
                {
                    if (memcmp(dataBuffer, messageB, strlen(messageB) + 1))
                    {
                        console_output(FALSE, "%s:\tDiskRead  (%s) -  Output match!\n", strArgs, device);
                    }
                    else
                    {
                        console_output(FALSE, "%s:\tDiskRead  (%s) -  Output mis-match!\n", strArgs, device);
                    }
                }
            }
        }
        else
        {
            memset(dataBuffer, 'a', THREADS_DISK_SECTOR_SIZE * testCases->sectorCount);
            if (options != 0)
            {
                if (options & DEVICES_OPTION_MESSAGEA)
                {
                    memcpy(dataBuffer, messageA, strlen(messageA) + 1);
                }
                else if (options & DEVICES_OPTION_MESSAGEB)
                {
                    memcpy(dataBuffer, messageB, strlen(messageB) + 1);
                }
            }

            DiskWrite(device, dataBuffer, testCases->platter, testCases->track, testCases->sectorStart, testCases->sectorCount, &status);
            console_output(FALSE, "%s:\tDiskWrite (%s) -  platter %d, track %02d, sectorStart %02d, sectorCount %02d, status 0x%08x\n",
                strArgs, device, testCases->platter, testCases->track, testCases->sectorStart, testCases->sectorCount, status);
        }
        free(dataBuffer);
        testCases++;
    }


    Exit(0);

    return 0;

}