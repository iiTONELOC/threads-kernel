#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"

/*********************************************************************************
*
* MessagingTest34
* 
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
    char* testName = GetTestName(__FILE__);
    char childNames[MAXPROC][256];
    int childId = 0;
    int mailboxId;

    memset(childNames, 0, sizeof(childNames));

    console_output(FALSE, "\n%s: started\n", testName);

    mailboxId = mailbox_create(10, 13);
    console_output(FALSE, "\n%s: mailbox_create returned id = %d\n", testName, mailboxId);

    char message[32];
    strcpy(message, "Test Message");

    console_output(FALSE, "\n%s: Sending message of max mailbox size\n", testName);
    int result = mailbox_send(mailboxId, message, 13, TRUE);
    if (result == 0)
    {
        console_output(FALSE, "%s: Delivered message: %s\n", testName, message);
    }
    else
    {
        console_output(FALSE, "%s: Message delivery failed: result = %d\n", testName, result);
    }

    message[0] = '\0';
    result = mailbox_receive(mailboxId, message, sizeof(message), TRUE);
    if (result == 13)
    {
        console_output(FALSE, "%s: Received message: %s\n", testName, message);
    }
    else
    {
        console_output(FALSE, "%s: Message receive failed: result = %d\n", testName, result);
    }


    k_exit(0);



    return 0;
} /* MessagingEntryPoint */