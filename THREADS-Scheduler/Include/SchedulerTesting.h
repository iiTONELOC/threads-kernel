#pragma once

#define BLOCK_UNBLOCK_COUNT 4
extern int gPid;

/* prototypes for functions used across multiple tests. */
int SimpleDelayExit(void* pArgs);
int SimpleBockExit(void* pArgs);
int SpawnTwoPriorityFour(char* strArgs);
int SpawnOnePriorityFour(char* strArgs);
int SpawnOnePriorityOne(char* strArgs);
int SpawnTwoPriorityTwo(char* strArgs);
int SignalAndJoinTwoLower(char* strArgs);
int DelayAndDump(char* strArgs);
int GetChildNumber(char* name);
void SystemDelay(int millisTime);
