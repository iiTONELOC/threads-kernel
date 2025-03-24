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

char* CreateSysCallsTestArgs(char* buffer, int bufferSize, char* prefix, int childId, int semHandle, int sempCount, int semvCount, int options)
{
    char* separator;

    snprintf(buffer, bufferSize, "%s-Child%d:%d-%d-%d-%d", prefix, childId, semHandle, sempCount, semvCount, options);

    separator = strchr(buffer, ':');

    return separator;
}



int StartDoSemsAndExit(char* strArgs)
{
    int i, result;
    char* separator;
    int sempCount = 0;
    int semvCount = 0;
    int options = 0;
    int bSemving = 0;
    int semHandle;
    int loopCount;
    int messageStartNumber = 0;
    int blocking = TRUE;

    /* parse the args */
    separator = strchr(strArgs, ':');
    if (separator != NULL)
    {
        separator[0] = '\0';
        separator++;
        sscanf(separator, "%d-%d-%d-%d", &semHandle, &sempCount, &semvCount, &options);
    }

    console_output(FALSE, "%s: started\n", strArgs);


    if (options & SYSCALL_TEST_OPTION_SEMP_FIRST)
    {
        loopCount = sempCount;
        bSemving = FALSE;
    }
    else
    {
        loopCount = semvCount;
        bSemving = TRUE;
    }

    for (int j = 0; j < 2; ++j)
    {
        for (i = 0; i < loopCount; i++)
        {
            if (bSemving)
            {
                console_output(FALSE, "%s: Calling SemV on semaphore %d\n", strArgs, semHandle);
                result = SemV(semHandle);
                console_output(FALSE, "%s: SemV returned %d\n", strArgs, result);
            }
            else
            {
                console_output(FALSE, "%s: Calling SemP on semaphore %d\n", strArgs, semHandle);
                result = SemP(semHandle);
                console_output(FALSE, "%s: SemP returned %d\n", strArgs, result);
            }
        }

        /* switch operations */
        if (options & SYSCALL_TEST_OPTION_SEMP_FIRST)
        {
            loopCount = semvCount;
            bSemving = TRUE;
        }
        else
        {
            loopCount = sempCount;
            bSemving = FALSE;
        }
    }

    if (options & SYSCALL_TEST_OPTION_SEMFREE)
    {
        console_output(FALSE, "%s: Calling SemFree on semaphore %d\n", strArgs, semHandle);
        result = SemFree(semHandle);
        console_output(FALSE, "%s: SemFree returned %d on semaphore %d\n", strArgs, result, semHandle);
    }

    console_output(FALSE, "%s: exiting\n", strArgs);

    Exit(9);

    return 0;
}