#pragma once


#define DEVICES_OPTION_MESSAGEA   1
#define DEVICES_OPTION_MESSAGEB   2

typedef struct
{
	int track;
	int platter;
	int sectorStart;
	int sectorCount;
	int disk;
	BOOL read;
} TestDiskParameters;

char* GetTestName(char* filename);
char* CreateDevicesTestArgs(char* buffer, int bufferSize, char* prefix, int childId, int sleepTime, TestDiskParameters *testList, int testListCount, unsigned char options);

int DevicesTestDriver(char* strArgs);