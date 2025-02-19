#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"

/*********************************************************************************
*
* MessagingTest01
*
* Simple test case that creates two mailboxes.
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
	int mbox_id;
	char* testName = GetTestName(__FILE__);

	console_output(FALSE, "\n%s: started\n", testName);
	
	mbox_id = mailbox_create(10, 50);
	console_output(FALSE, "%s: mailbox_create returned id = %d\n", testName, mbox_id);
	
	mbox_id = mailbox_create(20, 30);
	console_output(FALSE, "%s: mailbox_create returned id = %d\n", testName, mbox_id);

	k_exit(0);

	return 0; 
}
