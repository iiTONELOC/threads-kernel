#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"

int WaitForClockThenExit(char* strArgs);

/*********************************************************************************
*
* MessagingTest13
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    int status, kidpid;
    char nameBuffer[512];

    console_output(FALSE, "\n%s: started\n", testName);

    /* Use the -Child naming convention for the child process name. */
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);

    kidpid = k_spawn(nameBuffer, WaitForClockThenExit, nameBuffer, THREADS_MIN_STACK_SIZE, 4);

    kidpid = k_wait(&status);
    console_output(FALSE, "%s: Exit status for child %s is %d\n", testName, nameBuffer, status);

    k_exit(0);

    return 0;
} /* MessagingEntryPoint */

int WaitForClockThenExit(char* strArgs)
{
    int i, result;
    char buffer[20];
    int status;

    console_output(FALSE, "%s: started\n", strArgs);

    /* wait for the device to respond. */
    wait_device("clock", &status);

    console_output(FALSE, "\n%s: Returned from waitdevice, status: 0x%08x.\n", strArgs, status);

    k_exit(-3);

    return 0;
} /* SendSixAndExit */
