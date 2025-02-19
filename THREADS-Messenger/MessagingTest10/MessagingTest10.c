#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"

int SendOneAndExit(char* strArgs);
int CloseMailbox(char* strArgs);
int ReceiveSixAndExit(char* strArgs);

int mailboxId;


/*********************************************************************************
*
* MessagingTest10
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
	char* testName = GetTestName(__FILE__);
	int status, kidpid;
	char childNames[MAXPROC][256];
	char nameBuffer[512];
	int childId = 0;
	int mailboxId;
	char* optionSeparator;
	int messageCount = 0;

	memset(childNames, 0, sizeof(childNames));

	console_output(FALSE, "\n%s: started\n", testName);

	mailboxId = mailbox_create(5, 50);
	console_output(FALSE, "%s: mailbox_create returned id = %d\n", testName, mailboxId);

	/* 5 Sends - 0 Receives - No Options */
	optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 5, 0, OPTION_NONE);
	kidpid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
	optionSeparator[0] = '\0';
	strncpy(childNames[kidpid], nameBuffer, 256);
	messageCount = 5;

    /* 1 Sends - 0 Receives - NON BLOCKING */
    optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 1, 0, (messageCount << 8) | OPTION_START_NUMBER | OPTION_NON_BLOCKING);
    kidpid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidpid], nameBuffer, 256);
    messageCount++;

    /* 1 Sends - 0 Receives - No Options */
    optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 1, 0, (messageCount << 8) | OPTION_START_NUMBER | OPTION_NONE);
    kidpid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidpid], nameBuffer, 256);
    messageCount++;

    /* 1 Sends - 0 Receives - No Options */
    optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 1, 0, (messageCount << 8) | OPTION_START_NUMBER | OPTION_NONE);
    kidpid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidpid], nameBuffer, 256);

    /* 5 Sends - 0 Receives - No Options */
    optionSeparator = CreateMessageTestArgs(nameBuffer, sizeof(nameBuffer), testName, ++childId, mailboxId, 0, 6, OPTION_NONE);
    kidpid = k_spawn(nameBuffer, SendAndReceive, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
    optionSeparator[0] = '\0';
    strncpy(childNames[kidpid], nameBuffer, 256);

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

    return 0;
} /* MessagingEntryPoint */



int SendOneAndExit(char* strArgs)
{
    int result;
    char buffer[32];

    console_output(FALSE, "%s: started\n", strArgs);
    console_output(FALSE, "%s: Sending message to mailbox %d\n", strArgs, mailboxId);

    int testNumber = GetChildNumber(strArgs);

    sprintf(buffer, "Hello from child %d", testNumber);

    if (testNumber == 2)
    {
        result = mailbox_send(mailboxId, buffer, (int)strlen(buffer) + 1, TRUE);
        if (result == 0)
        {
            console_output(FALSE, "%s: mailbox_send Returned %d - Message '%s' DELIVERED\n", strArgs, result, buffer);
        }
    }
    else
    {
        console_output(FALSE, "%s: Non-blocking send to mailbox %d\n", strArgs, mailboxId);
        result = mailbox_send(mailboxId, buffer, (int)strlen(buffer) + 1, FALSE);

        console_output(FALSE, "%s: Non-blocking mailbox_send Returned %d\n", strArgs, result);
    }


    k_exit(-3);

    return 0;
} 

int ReceiveSixAndExit(char* strArgs)
{
    int i, result;
    char message[100];

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);

        for (i = 0; i <= 5; i++) {
            console_output(FALSE, "%s: Receiving message %d from mailbox %d\n", strArgs, i, mailboxId);
            result = mailbox_receive(mailboxId, message, sizeof(message), TRUE);
            console_output(FALSE, "%s: Received message %d, result = %d, message = %s\n", strArgs, i, result, message);
        }
    }

    k_exit(-3);

    return 0;
} 
