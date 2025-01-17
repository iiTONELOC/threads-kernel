#include <stdio.h>
#include "THREADSLib.h"
#include "SchedulerTesting.h"
#include "Scheduler.h"

int DelayLongAndExit(char* strArgs);

/*********************************************************************************
*
* SchedulerTest30
*
* Test of the readTime function
*
*********************************************************************************/
int SchedulerEntryPoint(void* pArgs)
{
    int status = -1, kidpid = -1;
    char* testName = "SchedulerTest30";
    char nameBuffer[512];

    console_output(FALSE, "\n%s: started\n", testName);

    for (int i = 0; i < 3; ++i)
    {
        console_output(FALSE, "%s: performing spawn of child\n", testName);
        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child%d", testName, i + 1);
        kidpid = k_spawn(nameBuffer, DelayLongAndExit, nameBuffer, THREADS_MIN_STACK_SIZE, 3);
        console_output(FALSE, "%s: after spawn of child with pid %d\n", testName, kidpid);
    }

    for (int i = 0; i < 3; ++i)
    {
        console_output(FALSE, "%s: waiting for child process\n", testName);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);
    }
    k_exit(0);

    return 0;
}


/*********************************************************************************
*
* DelayLongAndExit
*
* Delays based on the child number.
*
*********************************************************************************/
int DelayLongAndExit(char* strArgs)
{
    int testNumber; 

    if (strArgs != NULL)
    {
        int delayTime, readTimeResult;

        // Get the test number
        testNumber = GetChildNumber(strArgs);

        // calculate the delay
        delayTime = testNumber * 1000;

        console_output(FALSE, "%s: started\n", strArgs);
        
        SystemDelay(delayTime);

        if (strlen(strArgs) <= 0)
        {
            console_output(FALSE, "NO STRING: %d, %d\n", k_getpid(), testNumber);
            display_process_table();
            ExitProcess(0);
        }
        console_output(FALSE, "%s: calling readTime\n", strArgs);
        readTimeResult = read_time();
        console_output(FALSE, "%s: readTime returned %d\n", strArgs, readTimeResult);

        console_output(FALSE, "%s: quitting\n", strArgs);

    }

    k_exit(-3);

    return 0;
}