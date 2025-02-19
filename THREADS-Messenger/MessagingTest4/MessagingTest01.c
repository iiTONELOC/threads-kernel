#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"

/*********************************************************************************
*
* MessagingTest00
*
* Simple test case that creates one child process.
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
	int mbox_id;
	char* testName = "MessagingTest01";

	console_output(FALSE, "\n%s: started\n", testName);
	
	mbox_id = MboxCreate(10, 50);
	console_output(FALSE, "%s: MailBoxCreate returned id = %d\n", testName, mbox_id);
	
	mbox_id = MboxCreate(20, 30);
	console_output(FALSE, "%s: MailBoxCreate returned id = %d\n", testName, mbox_id);

	k_exit(0);
	return 0; /* so gcc will not complain about its absence... */
}
