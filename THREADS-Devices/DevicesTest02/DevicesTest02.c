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
* DevicesTest02
*
* Gets the disk information for both disks.
*
*********************************************************************************/
int DevicesEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    int sectorSize, platterCount, sectorCount, trackCount;


    console_output(FALSE, "\n%s: Started\n", testName);

    /* Get the disk information for disk 0 */
    console_output(FALSE, "%s:  Calling DiskInfo for disk0\n", testName);
    if (DiskInfo("disk0", &sectorSize, &sectorCount, &trackCount, &platterCount) == 0)
    {
        console_output(FALSE, "%s:  DiskInfo returned sectorSize %d, sectorCount %d, trackCount %d, platterCount %d\n", testName, sectorSize, sectorCount, trackCount, platterCount);
    }
    else
    {
        console_output(FALSE, "%s:  DiskInfo failed\n", testName);
    }

    /* Get the disk information for disk 1 */
    console_output(FALSE, "%s:  Calling DiskInfo for disk1\n", testName);
    if (DiskInfo("disk1", &sectorSize, &sectorCount, &trackCount, &platterCount) == 0)
    {
        console_output(FALSE, "%s:  DiskInfo returned sectorSize %d, sectorCount %d, trackCount %d, platterCount %d\n", testName, sectorSize, sectorCount, trackCount, platterCount);
    }
    else
    {
        console_output(FALSE, "%s:  DiskInfo failed\n", testName);
    }

    Exit(0);

    return 0;
}
