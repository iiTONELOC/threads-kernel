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
char* GetTestName(char* filename);


// #ifdef 1 // TODO: define __MESSAGING__
#define OPTION_NONE				0x00
#define OPTION_FREE_FIRST		0x01
#define OPTION_RECEIVE_FIRST	0x02
#define OPTION_NON_BLOCKING 	0x04
#define OPTION_START_NUMBER	    0x80

char* CreateMessageTestArgs(char* buffer, int bufferSize, char* prefix, int childId, int mailbox, int sendCount, int receiveCount, int options);
int SendAndReceive(char* strArgs);
// #endif 
