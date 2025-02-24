#include "Messenger.h"

#pragma once
#ifndef MESSAGING_PROCESS_H
#define MESSAGING_PROCESS_H

// __________________________ Function Prototypes __________________________
void InitEmptyMessagingProcessList();
MessagingProcess *FindProcessInTable(int byPid);
MessagingProcess *GetNextEmptyMessagingProcess();
void ResetMessagingProcess(MessagingProcess *pMessagingProcess);
int ReuseMessagingProcess(MessagingProcess *pMessagingProcess, int pid);
#endif
