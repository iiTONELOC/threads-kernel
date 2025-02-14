#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "LinkedProcessList.h"

Process *currentTestProcess;
Process testProcessTable[MAXPROC];
LinkedProcessList masterList[MAXPROC][NUM_UNIQUE_LISTS];

static void test_AddProcessToList(void);
static void test_InitializeProcessList(void);
static void test_GetNextPtrForListType(void);

int main(void)
{
    // init the master list
    memset(masterList, 0, sizeof(masterList));
    // init the process table
    memset(testProcessTable, 0, sizeof(testProcessTable));

    printf("\nRunning LinkedProcessList tests...\n");
    test_InitializeProcessList();
    test_AddProcessToList();
    return 0;
}

static void test_InitializeProcessList(void)
{
    LinkedProcessList list;
    InitializeProcessList(&list, UNINITIALIZED_LIST);

    assert(list.count == 0);
    assert(list.pHead == NULL);
    assert(list.pTail == NULL);
    assert(list.listType == UNINITIALIZED_LIST);

    printf(" 1.  test_InitializeProcessList passed\n");
}

static void test_AddProcessToList(void)
{
    //  lets create a single test process to add to the list
    int i;
    NewProcessArgs newProcessArgs = {0};
    Process *pNewProcess = &testProcessTable[0];

    for (i = 0; i < STATUS_QUIT + 1; i++)
    {
        newProcessArgs.pid = i + 1;
        newProcessArgs.arg = NULL;
        newProcessArgs.priority = 1;
        newProcessArgs.procSlot = i;
        newProcessArgs.entryPoint = 0;
        newProcessArgs.stacksize = 1024;
        newProcessArgs.name = "Test Process\0";
        newProcessArgs.pNewProcess = &testProcessTable[i];
       

        // initialize the new process
        InitializeNewProcess(&newProcessArgs);

        // initialize the process' master list
        for (int j = 0; j < NUM_UNIQUE_LISTS; j++)
        {
            InitializeProcessList(&masterList[i][j], j+2);
        }

        // if the process isn't the first in the list
        // add it to the first process' list
        if (i > 0)
        {
            AddProcessToList(pNewProcess, &masterList[0][i-1]);
        }
    }

    printf(" 2.  test_AddProcessToList passed\n");
}

// static void test_GetNextPtrForListType(void)
// {
//     printf(" 2.  test_GetNextPtrForListType passed\n");
// }
