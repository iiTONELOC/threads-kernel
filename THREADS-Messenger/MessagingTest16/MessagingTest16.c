#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"

/*********************************************************************************
*
* MessagingTest16
*
* Simple test case that creates one child process.
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
	int mboxids[50];
	char* testName = GetTestName(__FILE__);

	console_output(FALSE, "\n%s: started\n", testName);

	for (int i = 0; i < 50; i++) 
	{
		mboxids[i] = mailbox_create(10, 50);
		if (mboxids[i] < 0)
		{
			console_output(FALSE, "%s: mailbox_create returned id less than zero, id = %d\n",
				testName, mboxids[i]);
		}
	}

	k_exit(0);
	return 0; 
}
