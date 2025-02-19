#include <stdio.h>
#include "THREADSLib.h"
#include "Scheduler.h"
#include "Messaging.h"

int ReceiveTerminal(char* strArgs);
char inputChar;

/*********************************************************************************
*
* MessagingTest05
*
*
*********************************************************************************/
int MessagingEntryPoint(void* pArgs)
{
    char* testName = "MessagingTest05";
    int status, kidpid;
    char nameBuffer[512];
    device_control_block_t controlBlock;

    console_output(FALSE, "\n%s: started\n", testName);

    controlBlock.command = TERMINAL_READ_CHAR;
    controlBlock.input_data = &inputChar;
    controlBlock.data_length = 1;

    /* send a character to the terminal. */
    device_control("term0", controlBlock);

    /* Use the -Child naming convention for the child process name. */
    snprintf(nameBuffer, sizeof(nameBuffer), "%s-Child1", testName);
    kidpid = k_spawn(nameBuffer, ReceiveTerminal, nameBuffer, THREADS_MIN_STACK_SIZE, 3);

    kidpid = k_wait(&status);
    console_output(FALSE, "%s: exit status for child %d is %d\n", testName, kidpid, status);

    k_exit(0);

    return 0;
}

int ReceiveTerminal(char* strArgs)
{
    int result;
    int status=0;

    if (strArgs != NULL)
    {
        console_output(FALSE, "%s: started\n", strArgs);

        /* wait for the device to respond. */
        result = wait_device("term0", &status);
        console_output(FALSE, "\n%s: Returned from waitdevice, result: %d, status: 0x%08x.\n", strArgs, result, status);

    }

    k_exit(-3);

    return 0;
}

