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
* DevicesTest03
*
* Writes a buffer to disk 0 and reads it back.
*
*********************************************************************************/
int DevicesEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    int sectorSize, platterCount, sectorCount, trackCount;
    unsigned char outBuffer[THREADS_DISK_SECTOR_SIZE];
    unsigned char inBuffer[THREADS_DISK_SECTOR_SIZE];
    int status;

    srand(system_clock());
    memset(outBuffer, rand() % 0x100, sizeof(outBuffer));

    console_output(FALSE, "\n%s:\tStarted\n", testName);

    /* Get the disk information for disk 0 */
    console_output(FALSE, "%s:\tCalling DiskInfo for disk0\n", testName);
    if (DiskInfo("disk0", &sectorSize, &sectorCount, &trackCount, &platterCount) == 0)
    {
        console_output(FALSE, "%s:\tDiskInfo returned sectorSize %d, sectorCount %d, trackCount %d, platterCount %d\n", testName, sectorSize, sectorCount, trackCount, platterCount);
    }
    else
    {
        console_output(FALSE, "%s:\tDiskInfo failed\n", testName);
    }

    console_output(FALSE, "%s:\tWriting to disk 0, platter 0, track 5, sector 0\n", testName);
    DiskWrite("disk0", outBuffer, 0, 5, 0, 1, &status);
    console_output(FALSE, "%s:\tDiskWrite returned status = %d\n", testName, status);

    console_output(FALSE, "%s:\tReading from disk 0, platter 0, track 5, sector 0\n", testName);
    DiskRead("disk0", inBuffer, 0, 5, 0, 1, &status);
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
