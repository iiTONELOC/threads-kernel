#include "Messenger.h"
#include <string.h>
#pragma once
#ifndef MESSAGING_PROCESS_H
#define MESSAGING_PROCESS_H

// __________________________ Function Prototypes __________________________
void InitEmptyMessagingProcessArray();
MessagingProcess *FindProcessInTable(int byPid, bool setTimes);
void ResetMessagingProcess(MessagingProcess *pMessagingProcess);
#endif
