#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Process.h"

char nameBuffer[512] = {0};
Process testProcessTable[MAXPROC] = {0};

void test_CreateNewProcess(void);
void test_GetEmptyControlBlockIndex(void);

int main(void)
{

	printf("\nRunning Process tests...\n");
	test_CreateNewProcess();
	test_GetEmptyControlBlockIndex();
	return 0;
}

void test_CreateNewProcess(void)
{
	Process *pNewProcess = &testProcessTable[0];
	NewProcessArgs newProcessArgs = {
		.pid = 1,
		.priority = 1,
		.procSlot = 0,
		.entryPoint = 0,
		.stacksize = 1024,
		.arg = "Test Args\0",
		.name = "Test Name\0",
		.pNewProcess = pNewProcess};

	CreateNewProcess(&newProcessArgs);
	assert(pNewProcess->pid == 1);
	assert(pNewProcess->signal == 0);
	assert(pNewProcess->cpuTime == 0);
	assert(pNewProcess->priority == 1);
	assert(pNewProcess->exitCode == 0);
	assert(pNewProcess->startTime == 0);
	assert(pNewProcess->entryPoint == 0);
	assert(pNewProcess->context == NULL);
	assert(pNewProcess->elapsedTime == 0);
	assert(pNewProcess->joinStatus == -99);
	assert(pNewProcess->stacksize == 1024);
	assert(pNewProcess->status == STATUS_READY);
	assert(pNewProcess->processTableIndex == 0);
	assert(pNewProcess->quantum == MAX_PROC_QUANTUM);
	assert(strcmp(pNewProcess->name, "Test Name") == 0);
	assert(strcmp(pNewProcess->startArgs, "Test Args") == 0);

	printf(" 1.  test_CreateNewProcess passed\n");
}

void test_GetEmptyControlBlockIndex(void)
{
	int index;

	// we already have a process in the table
	index = GetEmptyControlBlockIndex(testProcessTable);
	assert(index == 1);

	// Add a new process to the table
	Process *pNewProcess = &testProcessTable[index];
	snprintf(nameBuffer, sizeof(nameBuffer), "TestProcess-%d", index);
	NewProcessArgs newProcessArgs = {
		.pid = index + 1,
		.priority = 1,
		.procSlot = 0,
		.entryPoint = 0,
		.stacksize = 1024,
		.name = nameBuffer,
		.pNewProcess = pNewProcess};

	CreateNewProcess(&newProcessArgs);

	// ensure the get empty control block index is working
	index = GetEmptyControlBlockIndex(testProcessTable);
	assert(index == 2);

	// remove the process from the table
	memset(&testProcessTable[1], 0, sizeof(Process));

	// ensure the get empty control block index is working
	index = GetEmptyControlBlockIndex(testProcessTable);
	assert(index == 1);

	printf(" 2.  test_GetEmptyControlBlockIndex passed\n");
}