#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "LinkedProcessList.h"

char nameBuffer[512] = {0};
Process *pNewTestProcess = NULL;
Process testProcessTable[MAXPROC];
LinkedProcessList masterList[MAXPROC][NUM_UNIQUE_LISTS];
int uniqueProcessLists[] = {PROCESS_CHILDREN_LIST, PROCESS_ZOMBIE_CHILDREN_LIST,
                            PROCESS_EXITING_CHILDREN_LIST, PROCESS_JOINING_PROCESSES_LIST};

static void test_PLI(void);
static void test_AddProcessToList(void);
static void test_PushProcessToList(void);
static void test_PopProcessFromList(void);
static void test_InitializeProcessList(void);
static void test_RemoveProcessFromList(void);

int main(void)
{
    // Init the master list
    memset(masterList, 0, sizeof(masterList));
    // Init the process table
    memset(testProcessTable, 0, sizeof(testProcessTable));

    printf("\nRunning LinkedProcessList tests...\n");
    test_PLI();
    test_InitializeProcessList();
    test_AddProcessToList();
    test_RemoveProcessFromList();
    test_PopProcessFromList();
    test_PushProcessToList();

    return 0;
}

static void test_PLI(void)
{
    int i = 0;
    int result;

    for (i; i < NUM_UNIQUE_LISTS; i++)
    {
        assert(PLI(uniqueProcessLists[i]) == i);
    }

    // ensure that the list is always in bounds regardless of the number provided
    for (i = 0; i < 10; i++)
    {
        result = PLI(i);

        if (i >= LIST_TYPE_TO_PROC_MASTER_OFFSET && i <= MAX_LIST_TYPES)
        {
            assert(result == i - LIST_TYPE_TO_PROC_MASTER_OFFSET);
        }
        else
        {
            assert(result == -1);
        }
    }

    printf(" 1.  test_PLI passed\n");
}

static void test_InitializeProcessList(void)
{
    LinkedProcessList list;
    InitializeProcessList(&list, UNINITIALIZED_LIST);

    assert(list.count == 0);
    assert(list.pHead == NULL);
    assert(list.pTail == NULL);
    assert(list.listType == UNINITIALIZED_LIST);

    printf(" 2.  test_InitializeProcessList passed\n");
}

static void test_AddProcessToList(void)
{
    int i;
    Process *pNewProcess = NULL;
    NewProcessArgs newProcessArgs = {0};

    // Create 5 processes
    for (i = 0; i < STATUS_QUIT; i++)
    {
        pNewProcess = &testProcessTable[i];
        newProcessArgs.pid = i + 1;
        newProcessArgs.arg = NULL;
        newProcessArgs.priority = 1;
        newProcessArgs.procSlot = i;
        newProcessArgs.entryPoint = 0;
        newProcessArgs.stacksize = 1024;
        newProcessArgs.pNewProcess = &testProcessTable[i];

        // Copy name into existing buffer
        snprintf(nameBuffer, sizeof(nameBuffer), "TestProcess-%d", i);
        newProcessArgs.name = nameBuffer;

        // Create the new process
        CreateNewProcess(&newProcessArgs);

        // Initialize the process' master list
        for (int j = 0; j < NUM_UNIQUE_LISTS; j++)
        {
            InitializeProcessList(&masterList[i][j], j + LIST_TYPE_TO_PROC_MASTER_OFFSET);
        }

        // If not the first process, add it to a list on the first process
        if (i > 0)
        {
            AddProcessToList(pNewProcess, &masterList[0][i - 1]);
        }
    }

    // Validate the first process' lists
    for (i = 0; i < NUM_UNIQUE_LISTS; i++)
    {
        assert(masterList[0][i].count == 1);
        assert(masterList[0][i].pHead == &testProcessTable[i + 1]);
        assert(masterList[0][i].pTail == &testProcessTable[i + 1]);
        assert(masterList[0][i].listType == uniqueProcessLists[i]);
    }

    printf(" 3.  test_AddProcessToList passed\n");
}

static void test_RemoveProcessFromList(void)
{
    Process *pProcess = &testProcessTable[0];

    // Create a new process
    pNewTestProcess = &testProcessTable[5];
    NewProcessArgs newProcessArgs = {
        .pid = 6,
        .arg = NULL,
        .priority = 1,
        .procSlot = 5,
        .entryPoint = 0,
        .stacksize = 1024,
        .pNewProcess = pNewTestProcess,
        .name = "TestProcess-6",
    };

    // Create the new process
    CreateNewProcess(&newProcessArgs);

    // for each list, add the new process to a list in pProcess and remove it
    for (int i = 0; i < NUM_UNIQUE_LISTS; i++)
    {
        // Add the new process to the list
        AddProcessToList(pNewTestProcess, &masterList[0][i]);

        // ensure the list now have 2 processes as they were not previously empty
        assert(masterList[0][i].count == 2);
        // verify the new process is at the tail of the list
        assert(masterList[0][i].pTail == pNewTestProcess);

        // Remove the new process from the list
        RemoveProcessFromList(&masterList[0][i], pNewTestProcess);

        // ensure the list now have 1 process as the new process was removed
        assert(masterList[0][i].count == 1);
        // verify the new process is no longer in the list
        assert(masterList[0][i].pTail != pNewTestProcess);
    }

    printf(" 4.  test_RemoveProcessFromList passed\n");
}

static void test_PopProcessFromList(void)
{
    int numProcessesPopped = 0;

    // Add the newTestProcess to the child list of the first process in the table
    AddProcessToList(pNewTestProcess, &masterList[0][PLI(PROCESS_CHILDREN_LIST)]);

    while (masterList[0][PLI(PROCESS_CHILDREN_LIST)].count > 0)
    {
        Process *poppedProcess = PopProcessFromList(&masterList[0][PLI(PROCESS_CHILDREN_LIST)]);
        numProcessesPopped++;

        // Ensure the process was popped from the list
        assert(poppedProcess != NULL);
    }

    // ensure 2 processes were popped from the list
    assert(numProcessesPopped == 2);

    printf(" 5.  test_PopProcessFromList passed\n");
}

static void test_PushProcessToList(void)
{

    Process *pTemp;
    Process *pProcess = &testProcessTable[0];

    // Create a new process
    pNewTestProcess = &testProcessTable[6];
    NewProcessArgs newProcessArgs = {
        .pid = 7,
        .arg = NULL,
        .priority = 1,
        .procSlot = 5,
        .entryPoint = 0,
        .stacksize = 1024,
        .pNewProcess = pNewTestProcess,
        .name = "TestProcess-7",
    };

    // Create the new process
    CreateNewProcess(&newProcessArgs);

    // add the process to the zombie list
    AddProcessToList(pNewTestProcess, &masterList[0][PLI(PROCESS_ZOMBIE_CHILDREN_LIST)]);

    // remove the first process from the zombie list, should not be the one we added
    pTemp = PopProcessFromList(&masterList[0][PLI(PROCESS_ZOMBIE_CHILDREN_LIST)]);

    // ensure the process was not the one we added
    assert(pTemp != pNewTestProcess);

    // add the zombie process back to the list using the push function
    PushProcessToList(&masterList[0][PLI(PROCESS_ZOMBIE_CHILDREN_LIST)], pNewTestProcess);

    // ensure the process is now at the head of the list
    assert(masterList[0][PLI(PROCESS_ZOMBIE_CHILDREN_LIST)].pHead == pNewTestProcess);

    printf(" 6.  test_PushProcessToList passed\n");
}
