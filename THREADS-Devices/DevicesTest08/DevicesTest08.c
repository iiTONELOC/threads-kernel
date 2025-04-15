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
* DevicesTest08
*
*
*
*********************************************************************************/
int DevicesEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    unsigned char outBuffer[THREADS_DISK_SECTOR_SIZE];
    int status;
    int result;

    srand(system_clock());
    memset(outBuffer, rand() % 0x100, sizeof(outBuffer));

    /* Test cases based on < 258 tracks. */

    console_output(FALSE, "\n%s:\tStarted\n", testName);

    console_output(FALSE, "%s:\tWriting to an invalid sector \n", testName);
    if ((result = DiskWrite("disk0", outBuffer, 0, 5, 17, 1, &status)) == 0)
    {
        console_output(FALSE, "%s:\tDiskWrite sector error test FAILED\n", testName);
    }
    else
    {
        console_output(FALSE, "%s:\tDiskWrite sector error test PASSED\n", testName);
    }

    console_output(FALSE, "%s:\tWriting to an invalid track \n", testName);
    if ((result = DiskWrite("disk0", outBuffer, 0, 258, 0, 1, &status)) == 0)
    {
        console_output(FALSE, "%s:\tDiskWrite track error test FAILED\n", testName);
    }
    else
    {
        console_output(FALSE, "%s:\tDiskWrite track error test PASSED\n", testName);
    }

    console_output(FALSE, "%s:\tWriting to an invalid platter \n", testName);
    if ((result = DiskWrite("disk0", outBuffer, 4, 25, 0, 1, &status)) == 0)
    {
        console_output(FALSE, "%s:\tDiskWrite track error test FAILED\n", testName);
    }
    else
    {
        console_output(FALSE, "%s:\tDiskWrite track error test PASSED\n", testName);
    }


    Exit(0);

    return 0;
}
