#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"

/*********************************************************************************
*
* MessagingTest37
* 
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

    int result = mailbox_send(mailboxId, message, 13, TRUE);
    if (result == 0)
    {
        console_output(FALSE, "%s: Delivered message: %s\n", testName, message);
    }
    else
    {
        console_output(FALSE, "%s: Message delivery failed: result = %d\n", testName, result);
    }

    console_output(FALSE, "\n%s: Receiving message with a buffer that is too small\n", testName);
    message[0] = '\0';
    result = mailbox_receive(mailboxId, message, 6, TRUE);
    if (result != -1)
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