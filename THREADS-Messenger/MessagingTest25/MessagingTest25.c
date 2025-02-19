#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "TestCommon.h"


/*********************************************************************************
*
* MessagingTest25
*
* Simple test case that creates one child process.
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
	int mbox_id;
    int i;
	char* testName = GetTestName(__FILE__);
    int tenMailboxes[10];
    int idCount = 0;

	console_output(FALSE, "\n%s: started\n", testName);

    console_output(FALSE, "\n%s: trying to create too many mailboxes\n", testName);

    for (i = 0; i < MAXMBOX - 5; i++) 
    {
        mbox_id = mailbox_create(10, 50);
        if (mbox_id < 0)
        {
            console_output(FALSE, "\n%s: mailbox_create returned id less than zero, id = %d\n", testName, mbox_id);
        }
        else
        {
            if (i >= 20 && i < 30)
            {
                tenMailboxes[i - 20] = mbox_id;
                idCount++;
            }
        }
    }

    for (i = 0; i < idCount; i++)
    {
        int result;

        result = mailbox_free(tenMailboxes[i]);
        console_output(FALSE, "\n%s: mailbox_close returned %d\n", testName, result);
    }

    for (i = 0; i < 14; i++)
    {
        mbox_id = mailbox_create(10, 50);
        if (mbox_id < 0)
        {
            console_output(FALSE, "\n%s: mailbox_create returned id less than zero, id = %d\n", testName, mbox_id);
        }
        else
        {
            console_output(FALSE, "\n%s: mailbox_create returned id = %d\n", testName, mbox_id);
        }
    }

	k_exit(0);
	return 0; 
}
