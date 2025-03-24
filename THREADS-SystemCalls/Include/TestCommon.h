#pragma once

#define SYSCALL_TEST_OPTION_SEMP_FIRST     1
#define SYSCALL_TEST_OPTION_SEMV_FIRST     2
#define SYSCALL_TEST_OPTION_SEMFREE        4

char* GetTestName(char* filename);
char* CreateSysCallsTestArgs(char* buffer, int bufferSize, char* prefix, int childId, int semHandle, int sempCount, int semvCount, int options);
int StartDoSemsAndExit(char* strArgs);

