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
* DevicesTest05
*
* Writes and reads across 3 sectors on both disk0 and disk1.
*
*********************************************************************************/
int DevicesEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    unsigned char outBuffer[THREADS_DISK_SECTOR_SIZE * 3];
    unsigned char inBuffer[THREADS_DISK_SECTOR_SIZE * 3];
    int status;

    /* Initialize the input and output buffers */
    srand(system_clock());
    memset(outBuffer, rand() % 0x100, THREADS_DISK_SECTOR_SIZE);
    memset(outBuffer + THREADS_DISK_SECTOR_SIZE, rand() % 0x100, THREADS_DISK_SECTOR_SIZE);
    memset(outBuffer + (THREADS_DISK_SECTOR_SIZE * 2), rand() % 0x100, THREADS_DISK_SECTOR_SIZE);

    memset(inBuffer, 0, sizeof(inBuffer));

    console_output(FALSE, "\n%s:\tStarted\n", testName);

    console_output(FALSE, "%s:\tWriting to disk 0, platter 0, track 1, sectors 1-3\n", testName);
    DiskWrite("disk0", outBuffer, 0, 1, 1, 3, &status);
    console_output(FALSE, "%s:\tDiskWrite returned status = %d\n", testName, status);

    console_output(FALSE, "%s:\tWriting to disk 1, platter 1, track 2, sectors 8-12\n", testName);
    DiskWrite("disk1", outBuffer, 1, 2, 8, 3, &status);
    console_output(FALSE, "%s:\tDiskWrite returned status = %d\n", testName, status);

    console_output(FALSE, "%s:\tReading from disk 0, platter 0, track 6, sectors 3-6\n", testName);
    DiskRead("disk0", inBuffer, 0, 1, 1, 3, &status);
    console_output(FALSE, "%s:\tDiskRead returned status = %d\n", testName, status);

    if (memcmp(outBuffer, inBuffer, sizeof(outBuffer)) == 0)
    {
        console_output(FALSE, "%s:\tIn and Out buffers match!\n", testName);
    }
    else
    {
        console_output(FALSE, "%s:\tIn and Out buffers do not match\n", testName);
    }

    /* reset the in buffer */
    memset(inBuffer, 0, sizeof(inBuffer));

    console_output(FALSE, "%s:\tReading from disk 1, platter 1, track 2, sectors 8-12\n", testName);
    DiskRead("disk1", inBuffer, 1, 2, 8, 3, &status);
    console_output(FALSE, "%s:\tDiskRead returned status = %d\n", testName, status);

    if (memcmp(outBuffer, inBuffer, sizeof(outBuffer)) == 0)
    {
        console_output(FALSE, "%s:\tIn and Out buffers match!\n", testName);
    }
    else
    {
        console_output(FALSE, "%s:\tIn and Out buffers do not match\n", testName);
    }

    Exit(0);

    return 0;
}
