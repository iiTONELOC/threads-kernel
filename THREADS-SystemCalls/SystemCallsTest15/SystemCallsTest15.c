#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "SystemCalls.h"
#include "TestCommon.h"
#include "libuser.h"

int SpawnOneWithGrandchildOneWithout(void* strArgs);
int SpawnThreeSimple(void* strArgs);

/*********************************************************************************
*
* SystemCallsTest15
*
*
*********************************************************************************/
int SystemCallsEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    int status = -1, pid, kidpid = -1;
    char nameBuffer[512];

    /* Just output a message and exit. */
    console_output(FALSE, "\n%s: started\n", testName);

    /* Use the -Child naming convention for the child process name. */
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);    
    Spawn(nameBuffer, SpawnOneWithGrandchildOneWithout, nameBuffer, THREADS_MIN_STACK_SIZE, 2, &pid);
    console_output(FALSE, "%s: after spawn of child with PID %d\n", testName, pid);

    Wait(&pid, &status);
    console_output(FALSE, "%s: Wait returned for child with PID %d and status %d\n", testName, pid, status);

    console_output(FALSE, "%s: Parent done. Calling Exit.\n", testName);
    Exit(8);

    return 0;
}


int SpawnOneWithGrandchildOneWithout(void* strArgs)
{
    char nameBuffer[512];
    int childId = 1;
    int pid;
    int status;
    char* optionSeparator;

    /* Just output a message and exit. */
    console_output(FALSE, "%s: started\n", strArgs);

    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", strArgs);
    Spawn(nameBuffer, SpawnThreeSimple, nameBuffer, THREADS_MIN_STACK_SIZE, 4, &pid);
    console_output(FALSE, "%s: after spawn of %d\n", strArgs, pid);

    Wait(&pid, &status);
    console_output(FALSE, "%s: Wait returned for child with PID %d and status %d\n", strArgs, pid, status);

    /* 0 Semps - 0 Semvs - No options */
    optionSeparator = CreateSysCallsTestArgs(nameBuffer, sizeof(nameBuffer), strArgs, ++childId, 0, 0, 0, 0);
    Spawn(nameBuffer, StartDoSemsAndExit, nameBuffer, THREADS_MIN_STACK_SIZE, 1, &pid);
    console_output(FALSE, "%s: after spawn of %d\n", strArgs, pid);

    Wait(&pid, &status);
    console_output(FALSE, "%s: Wait returned for child with PID %d and status %d\n", strArgs, pid, status);

    console_output(FALSE, "%s: Parent done. Calling Exit.\n", strArgs);

    Exit(9);

    return 0;


}


int SpawnThreeSimple(void* strArgs)
{
    char nameBuffer[512];
    int childId = 0;
    int pid;
    int status;
    char* optionSeparator;

    /* Just output a message and exit. */
    console_output(FALSE, "%s: started\n", strArgs);

    /* 0 Semps - 0 Semvs - No Options */
    optionSeparator = CreateSysCallsTestArgs(nameBuffer, sizeof(nameBuffer), strArgs, ++childId, 0, 0, 0, 0);
    Spawn(nameBuffer, StartDoSemsAndExit, nameBuffer, THREADS_MIN_STACK_SIZE, 2, &pid);
    console_output(FALSE, "%s: after spawn of %d\n", strArgs, pid);

    /* 0 Semps - 0 Semvs - No Options */
    optionSeparator = CreateSysCallsTestArgs(nameBuffer, sizeof(nameBuffer), strArgs, ++childId, 0, 0, 0, 0);
    Spawn(nameBuffer, StartDoSemsAndExit, nameBuffer, THREADS_MIN_STACK_SIZE, 2, &pid);
    console_output(FALSE, "%s: after spawn of %d\n", strArgs, pid);

    /* 0 Semps - 0 Semvs - No Options */
    optionSeparator = CreateSysCallsTestArgs(nameBuffer, sizeof(nameBuffer), strArgs, ++childId, 0, 0, 0, 0);
    Spawn(nameBuffer, StartDoSemsAndExit, nameBuffer, THREADS_MIN_STACK_SIZE, 2, &pid);
    console_output(FALSE, "%s: after spawn of %d\n", strArgs, pid);


    Exit(10);
}