#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"



/*********************************************************************************
*
* MessagingTest27
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    int status, kidpid, testPid;
    char childNames[MAXPROC][256];
    char nameBuffer[512];
    int childId = 0;
    int mailboxId;
    char* optionSeparator;

    memset(childNames, 0, sizeof(childNames));

    console_output(FALSE, "\n%s: started\n", testName);

    mailboxId = mailbox_create(5, MAX_MESSAGE);
    console_output(FALSE, "\n%s: mailbox_create returned id = %d\n", testName, mailboxId);

    optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 2, 2, OPTION_RECEIVE_FIRST);
    kidpid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 1);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidpid], nameBuffer, 256);
    console_output(FALSE, "%s: Spawned %s with priority 4\n", testName, nameBuffer);

    optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 0, 1, OPTION_NONE);
    kidpid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidpid], nameBuffer, 256);
    console_output(FALSE, "%s: Spawned %s with priority 3\n", testName, nameBuffer);

    optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 0, 1, OPTION_NONE);
    kidpid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidpid], nameBuffer, 256);
    console_output(FALSE, "%s: Spawned %s with priority 2\n", testName, nameBuffer);

    optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 0, 1, OPTION_NONE);
    kidpid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 4);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidpid], nameBuffer, 256);
    console_output(FALSE, "%s: Spawned %s with priority 3\n", testName, nameBuffer);

    /* spawn a process that just exits */
    optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 0, 0, OPTION_NONE);
    testPid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 1);
    optionSeparator[0] = '\0';
    strncpy(childNames[testPid], nameBuffer, 256);
    console_output(FALSE, "%s: Spawned %s with priority 3\n", testName, nameBuffer);

    kidpid = k_wait(&status);
    if (kidpid != testPid)
    {
        console_output(FALSE, "%s: Priority test FAILED, pid values do not match.\n", testName);

    }
    console_output(FALSE, "%s: Exit status for child %s is %d\n", testName, childNames[kidpid], status);


    optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 0, 0, OPTION_FREE_FIRST);
    kidpid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 1);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidpid], nameBuffer, 256);
    console_output(FALSE, "%s: Spawned %s with priority 1\n", testName, nameBuffer);

    kidpid = k_wait(&status);
    console_output(FALSE, "%s: Exit status for child %s is %d\n", testName, childNames[kidpid], status);

    kidpid = k_wait(&status);
    console_output(FALSE, "%s: Exit status for child %s is %d\n", testName, childNames[kidpid], status);

    kidpid = k_wait(&status);
    console_output(FALSE, "%s: Exit status for child %s is %d\n", testName, childNames[kidpid], status);

    kidpid = k_wait(&status);
    console_output(FALSE, "%s: Exit status for child %s is %d\n", testName, childNames[kidpid], status);

    kidpid = k_wait(&status);
    console_output(FALSE, "%s: Exit status for child %s is %d\n", testName, childNames[kidpid], status);

    k_exit(0);

    console_output(FALSE, "\n%s: started\n", testName);


    return 0;
} /* MessagingEntryPoint */

