#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"


/*********************************************************************************
*
* MessagingTest15
*
* Send a message that is too large
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
	int mailboxId;
	char* testName = GetTestName(__FILE__);
	int result;
	char message[80];

	console_output(FALSE, "\n%s: started\n", testName);

	mailboxId = mailbox_create(10, 5);
	console_output(FALSE, "\n%s: mailbox_create returned id = %d\n", testName, mailboxId);

	sprintf(message, "Hello There");

	console_output(FALSE, "%s: Sending large message to mailbox %d\n", testName, mailboxId);
	result = mailbox_send(mailboxId, message, (int)strlen(message) + 1, TRUE);

	if (result != 0) {
		console_output(FALSE, "%s: mailbox_send failed\n", testName);
		k_exit(0);
		return 0;
	}

	console_output(FALSE, "%s: mailbox_send returned %d for message = %s\n", testName, result, message);


	memset(message, 0, sizeof(message));

	console_output(FALSE, "%s: Receiving message from mailbox %d\n", testName, mailboxId);
	result = mailbox_receive(mailboxId, message, sizeof(message), TRUE);
	if (result >= 0)
	{
		console_output(FALSE, "%s: Received message, result = %d, message = %s\n", testName, result, message);
	}
	else
	{
		console_output(FALSE, "%s: mailbox_receive returned, result = %d\n", testName, result);
	}

	k_exit(0);

	return 0;
}

