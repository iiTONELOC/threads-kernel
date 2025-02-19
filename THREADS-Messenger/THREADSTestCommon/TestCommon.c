#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "THREADSLib.h"
#include "TestCommon.h"
#include "Scheduler.h"
#include "Messaging.h"

int gPid = -1;

/* prototypes for functions used across multiple tests. */
int SimpleDelayExit(void* pArgs);
int SpawnTwoPriorityFour(char* pArgs);
int DelayAndDump(char* arg);
int SignalAndJoinTwoLower(char* strArgs);

static testNameBuffer[512];


char* CreateMessageTestArgs(char* buffer, int bufferSize, char* prefix, int childId, int mailbox, int sendCount, int receiveCount, int options)
{
    char* separator;

    snprintf(buffer, bufferSize, "%s-Child%d:%d-%d-%d-%d", prefix, childId, mailbox, sendCount, receiveCount, options);

    separator = strchr(buffer, ':');

    return separator;
}


int SendAndReceive(char* strArgs)
{
    int i, result;
    char message[MAX_MESSAGE];
    char* separator;
    int sendCount = 0;
    int receiveCount = 0;
    int options = 0;
    int bSending=0;
    int mailbox;
    int loopCount;
    int messageStartNumber=0;
    int blocking = TRUE;

    /* parse the args */
    separator = strchr(strArgs, ':');
    separator[0] = '\0';
    separator++;
    sscanf(separator, "%d-%d-%d-%d", &mailbox, &sendCount, &receiveCount, &options);

    console_output(FALSE, "%s: started\n", strArgs);

    if (options & OPTION_NON_BLOCKING)
    {
        // get the high low order byte and start the message number with the value
        blocking = FALSE;
    }

    if (options & OPTION_START_NUMBER)
    {
        // get the high low order byte and start the message number with the value
        messageStartNumber = options >> 8 & 0x000000FF;
    }
    /* see if the free first option is set. */
    if (options & OPTION_FREE_FIRST)
    {
        result = mailbox_free(mailbox);

        console_output(FALSE, "%s: mailbox_free returned %d\n", strArgs, result);
    }

    if (options & OPTION_RECEIVE_FIRST)
    {
        loopCount = receiveCount;
        bSending = FALSE;
    }
    else
    {
        loopCount = sendCount;
        bSending = TRUE;
    }

    for (int j = 0; j < 2; ++j)
    {
        for (i = 0; i < loopCount; i++)
        {
            if (bSending)
            {
                sprintf(message, "Message Number %d", i + messageStartNumber);
                result = mailbox_send(mailbox, message, (int)strlen(message) + 1, blocking);
                if (result == 0)
                {
                    console_output(FALSE, "%s: Delivered message: %s\n", strArgs, message);
                }
                else
                {
                    console_output(FALSE, "%s: Message delivery failed: result = %d\n", strArgs, result);
                }
            }
            else
            {
                result = mailbox_receive(mailbox, message, MAX_MESSAGE, blocking);
                if (result >= 0)
                {
                    console_output(FALSE, "%s: Received message: %s\n", strArgs, message);
                }
                else
                {
                    console_output(FALSE, "%s: Message receive failed: result = %d\n", strArgs, result);
                }
            }
        }

        /* switch operations */
        if (options & OPTION_RECEIVE_FIRST)
        {
            loopCount = sendCount;
            bSending = TRUE;
        }
        else
        {
            loopCount = receiveCount;
            bSending = FALSE;
        }

    }

    k_exit(-3);

    return 0;
}

/* Can only get this for the test name since there is one buffer for this. */
char* GetTestName(char* filename)
{
    char* testName;
    
    testName = filename;
    if (strrchr(filename, '\\'))
    {
        testName = strrchr(filename, '\\') + 1;
    }

    strncpy((char *)testNameBuffer, testName, strlen(testName) - 2);

    return (char*)testNameBuffer;
}
/*
*  SystemDelay - busy wait delay for the specified time.
*/
#pragma optimize( "", off )
void SystemDelay(int millisTime)
{
    unsigned int startTime;
    unsigned int currentTime;

    currentTime = startTime = read_clock() / 1000;
    while ((currentTime - startTime) < (unsigned int)millisTime)
    {
        currentTime = read_clock() / 1000;
    }
}
#pragma optimize( "", on )

/*
*  DelayAndDump - delays and with dumps process if name ends 
*                 in a value divisible by 4.
*/
int DelayAndDump(char* arg)
{
    unsigned long startTime, currentTime;
    int printingThread = 0;
    unsigned long  printAt = 2500;
    unsigned long  stopAt = 10000;
    char* testName = __func__;
    int testNumber;

    console_output(FALSE, "%s: started\n", arg);

    // Get the test number
    testNumber = GetChildNumber(arg);

    console_output(FALSE, "%s: started, child number is %d\n", arg, testNumber);

    if ((testNumber % 4) == 0)
    {
        printingThread = 1;
    }

    startTime = read_clock() / 1000;
    while (1)
    {
        currentTime = read_clock() / 1000;

        if ((printingThread) && (currentTime - startTime) > printAt)
        {
            printAt += 5000;
            display_process_table();
        }
        else if ((currentTime - startTime) > stopAt)
        {
            break;
        }

    }
    if (strlen(arg) <= 0)
    {
        console_output(FALSE, "NO STRING: %d, %d\n", k_getpid(), testNumber);
    }
    console_output(FALSE, "%s: exiting, pid = %d\n", arg, k_getpid());
    k_exit(-k_getpid());
    return 0;
}


/*********************************************************************************
*
* SimpleDelayExit
*
* Simple test process that delays and exits.
*
*********************************************************************************/
int SimpleDelayExit(void* pArgs)
{
    if (pArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", (char*)pArgs);
        SystemDelay(10);
        console_output(FALSE, "%s: quitting\n", (char*)pArgs);
    }

    k_exit(-3);

    return 0;
}

/*********************************************************************************
*
*  SimpleBockExit - Blocks then exits.
*                 
*******************************************************************************/
int SimpleBockExit(void* pArgs)
{
    if (pArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", (char*)pArgs);
        block(14);
        console_output(FALSE, "%s: quitting\n", (char*)pArgs);
    }

    k_exit(-3);

    return 0;
}

/*********************************************************************************
*
*  SpawnTwoPriorityFour - Spawns two priority 4 child processes.
*
*******************************************************************************/
int SpawnTwoPriorityFour(char* strArgs)
{
    int kidpid;
    int status = 0xff;
    char nameBuffer[1024];

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);
        console_output(FALSE, "%s: performing spawn of first child\n", strArgs);

        /* Use the -Child naming convention for the child process name. */
        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", strArgs);
        kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 4);
        console_output(FALSE, "%s: spawn of first child returned pid = %d\n", strArgs, kidpid);

        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child2", strArgs);
        console_output(FALSE, "%s: performing spawn of second child\n", strArgs);
        kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 4);
        console_output(FALSE, "%s: spawn of second child returned pid = %d\n", strArgs, kidpid);

        console_output(FALSE, "%s: performing first wait.\n", strArgs);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs,
            kidpid, status);

        console_output(FALSE, "%s: performing second wait.\n", strArgs);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs,
            kidpid, status);
    }
    k_exit(-3);

    return 0;
}

/*********************************************************************************
*
*  SpawnTwoPriorityTwo - Spawns two priority 2 child processes.
*
*******************************************************************************/
int SpawnTwoPriorityTwo(char* strArgs)
{
    int kidpid;
    int status = 0xff;
    char nameBuffer[1024];

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);
        console_output(FALSE, "%s: performing spawn of first child\n", strArgs);

        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", strArgs);
        kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
        console_output(FALSE, "%s: spawn of first child returned pid = %d\n", strArgs, kidpid);

        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child2", strArgs);
        console_output(FALSE, "%s: performing spawn of second child\n", strArgs);
        kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
        console_output(FALSE, "%s: spawn of second child returned pid = %d\n", strArgs, kidpid);

        console_output(FALSE, "%s: performing first wait\n", strArgs);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs,
            kidpid, status);

        console_output(FALSE, "%s: performing second wait\n", strArgs);
        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs,
            kidpid, status);
    }
    k_exit(1);

    return 0;
}

/*********************************************************************************
*
*  SpawnOnePriorityFour - Spawns one priority 4 child process.
*
*******************************************************************************/
int SpawnOnePriorityFour(char* strArgs)
{
    int kidpid;
    int status = 0xff;
    char nameBuffer[1024];

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);
        console_output(FALSE, "%s: performing spawn of child\n", strArgs);

        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", strArgs);
        kidpid = k_spawn(nameBuffer, SimpleDelayExit, nameBuffer, THREADS_MIN_STACK_SIZE, 4);
        console_output(FALSE, "%s: spawn of child returned pid = %d\n", strArgs, kidpid);

        kidpid = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs,
            kidpid, status);
    }
    k_exit(-3);

    return 0;
}

/*********************************************************************************
*
*  SpawnOnePriorityOne - Spawns one priority 1 child process.
*
*******************************************************************************/
int SpawnOnePriorityOne(char* strArgs)
{
    int kidpid;
    int status = 0xff;
    char testName[1024];

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);
        console_output(FALSE, "%s: performing spawn of child\n", strArgs);

        snprintf(testName, sizeof(testName), "%s-Child1", strArgs);
        kidpid = k_spawn(testName, SimpleDelayExit, testName, THREADS_MIN_STACK_SIZE, 1);
        console_output(FALSE, "%s: spawn of child returned pid = %d\n", strArgs, kidpid);

        kidpid = k_wait(&status);
        if (kidpid == -1)
        {
            console_output(FALSE, "%s: process was signaled  during wait()\n", testName);
        }
        else
        {
            console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);
        }
    }
    k_exit(-3);

    return 0;
}

/*********************************************************************************
*
*  SignalAndJoinTwoLower - Spawns two processes then signals then and joins them
*
*******************************************************************************/
int SignalAndJoinTwoLower(char* strArgs)
{
    int child1, child2;
    int status = 0xff;
    char nameBuffer[1024];
    int exitCode;

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);

        console_output(FALSE, "%s: performing spawn of first child\n", strArgs);
        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", strArgs);
        child1 = k_spawn(nameBuffer, SpawnOnePriorityOne, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
        console_output(FALSE, "%s: spawn of first child returned pid = %d\n", strArgs, child1);

        console_output(FALSE, "%s: performing spawn of second child\n", strArgs);
        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child2", strArgs);
        child2 = k_spawn(nameBuffer, SpawnOnePriorityOne, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
        console_output(FALSE, "%s: spawn of first child returned pid = %d\n", strArgs, child2);

        display_process_table();

        console_output(FALSE, "%s: signaling first child\n", strArgs);
        k_kill(child1, SIG_TERM);
        k_join(child1, &exitCode);
        console_output(FALSE, "%s: after joining first child, status = %d\n", strArgs, exitCode);

        display_process_table();

        console_output(FALSE, "%s: signaling second child\n", strArgs);
        k_kill(child2, SIG_TERM);
        k_join(child2, &exitCode);
        console_output(FALSE, "%s: after joining second child, status = %d\n", strArgs, exitCode);

        display_process_table();

        console_output(FALSE, "%s: performing joins\n", strArgs);
        child1 = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs,
            child1, status);

        child2 = k_wait(&status);
        console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs,
            child2, status);
    }
    k_exit(-3);
    return 0;
}


/*
*  Returns the integer at the end of name.
*/
int GetChildNumber(char* name)
{
    int intIndex;
    int testNumber = 0;

    // Simple extraction of the test number
    intIndex = (int)strlen(name) - 1;
    while (intIndex >= 0 && isdigit(name[intIndex]))
    {
        intIndex--;
    }
    testNumber = atoi(&name[intIndex + 1]);

    return testNumber;
}

int pidToJoin;
int pids[BLOCK_UNBLOCK_COUNT];
int UnblockTwoPriorities(char* strArgs)
{
    int status = 0;

    console_output(FALSE, "%s: started\n", strArgs);

    for (int i = 0; i < BLOCK_UNBLOCK_COUNT * 2; ++i)
    {
        console_output(FALSE, "%s: Unblocking process %d\n", strArgs, pids[i]);
        unblock(pids[i]);
    }

    k_exit(-2);

    return 0;
}

int SpawnJoiner(char* strArgs)
{
    int status = 0xff;
    int result;

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);
        console_output(FALSE, "%s: joining target\n", strArgs);
        result = k_join(pidToJoin, &status);
        console_output(FALSE, "%s: k_join for pid %d returned %d with status %d\n", strArgs, pidToJoin, result, status);

    }
    k_exit(-2);

    return 0;

}

int DumpProcess(char* strArgs)
{
    int status = 0xff;

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);
        display_process_table();
    }
    k_exit(-3);

    return 0;
}

int SpawnDumpProcess(char* strArgs)
{
    int kidpid;
    int status = 0xff;
    char nameBuffer[1024];

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);
        console_output(FALSE, "%s: performing spawn of child\n", strArgs);

        snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", strArgs);
        kidpid = k_spawn(nameBuffer, DumpProcess, nameBuffer, THREADS_MIN_STACK_SIZE, 2);
        console_output(FALSE, "%s: spawn of child returned pid = %d\n", strArgs, kidpid);
        /* If the global pid is not set, then set it to kidpid. */
        if (gPid == -1)
        {
            gPid = kidpid;
        }

        kidpid = k_wait(&status);
        if (kidpid == -5)
        {
            console_output(FALSE, "%s: process was signaled during wait\n", strArgs);
        }
        else
        {
            console_output(FALSE, "%s: exit status for child %d is %d\n", strArgs,
                kidpid, status);
        }
    }
    k_exit(-3);

    return 0;
}


int SignalJoinGlobalPid(char* arg)
{
    int exitCode;

    if (arg != NULL)
    {
        console_output(FALSE, "%s: started\n", arg);
        console_output(FALSE, "%s: signaling %d.\n", arg, gPid);
        k_kill(gPid, SIG_TERM);
        console_output(FALSE, "%s: joining %d.\n", arg, gPid);
        k_join(gPid, &exitCode);
        console_output(FALSE, "%s: after joining child, pid %d, status = %d\n", arg, gPid, exitCode);
    }

    k_exit(5);
    return 0;
}

int JoinProcess(char* arg)
{
    int exitCode;

    if (arg != NULL)
    {
        console_output(FALSE, "%s: started\n", arg);
        console_output(FALSE, "%s: joining %d\n", arg, gPid);
        k_join(gPid, &exitCode);
        console_output(FALSE, "%s: after joining child, pid %d, status = %d\n", arg, gPid, exitCode);
    }

    k_exit(5);
    return 0;
}