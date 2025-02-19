#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"

/*********************************************************************************
*
* MessagingTest02
*
* Creates more than the maximum number of mailboxes.
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
	int mbox_id;
	char* testName = GetTestName(__FILE__);


	console_output(FALSE, "\n%s: started\n", testName);
	
	for (int i = 0; i < MAXMBOX + 3; i++) 
	{
		mbox_id = mailbox_create(10, 50);
		if (mbox_id < 0)
		{
			console_output(FALSE, "%s: mailbox_create returned id less than zero, id = %d\n",
				testName, mbox_id);
		}
	}

	k_exit(0);
	return 0; 
}
