

#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"


/*********************************************************************************
*
* MessagingTest03
*
* Simple mailbox send and receive test.
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
	int mailboxId;
	char* testName = GetTestName(__FILE__);
	int result;
	char buffer[80];

	memset(buffer, 0, sizeof(buffer));
	console_output(FALSE, "\n%s: started\n", testName);
	
	mailboxId = mailbox_create(10, 50);
	printf("%s: mailbox_create returned id = %d\n", testName, mailboxId);

	printf("%s: sending message to mailbox %d\n", testName, mailboxId);
	result = mailbox_send(mailboxId, "hello there", 12, TRUE);
	printf("%s: after send of message, result = %d\n", testName, result);

	console_output(FALSE, "%s: receiving message from mailbox %d\n", testName, mailboxId);
	result = mailbox_receive(mailboxId, buffer, sizeof(buffer), TRUE);
	console_output(FALSE, "%s: after receipt of message, result = %d\n", testName, result);
	console_output(FALSE, "%s:   message received = %s\n", testName, buffer);

	k_exit(0);

	return 0; 
}

