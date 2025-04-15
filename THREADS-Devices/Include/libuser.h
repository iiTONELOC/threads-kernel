/*
 * This file contains the function definitions for the library interfaces
 * to the THREADS system calls.
 */
#ifndef _LIBUSER_H
#define _LIBUSER_H

/* Prototypes for user-level system calls. */
extern int  Spawn(char *name, int (*func)(char *), char *arg, int stack_size,
                  int priority, int *pid);
extern int  Wait(int *pid, int *status);
extern void Exit(int status);
extern void GetTimeofDay(int *tod);
extern void CPUTime(int *cpu);
extern void GetPID(int *pid);
extern int  SemCreate(int value, int *semaphore);
extern int  SemP(int semaphore);
extern int  SemV(int semaphore);
extern int  SemFree(int semaphore);

/* System Call -- User Function Prototypes */
extern int  SleepSeconds(int seconds);
extern int  DiskRead(char* deviceName, void* dataBuffer, int platter, int track, int firstSector, int sectors, int* status);
extern int  DiskWrite(char* deviceName, void* dataBuffer, int platter, int track, int firstSector, int sectors, int* status);
extern int  DiskInfo(char* deviceName, int* sectorSize, int* sectorCount, int* trackCount, int* platterCount);
extern int  TermRead(char *buff, int bsize, int unit_id, int *nread);
extern int  TermWrite(char *buff, int bsize, int unit_id, int *nwrite);

/* Interface to Phase 2 mailbox routines */
extern int  Mbox_Create(int numslots, int slotsize, int *mbox);
extern int  Mbox_Release(int mbox);
extern int  Mbox_Send(int mbox, int size, void *msg);
extern int  Mbox_Receive(int mbox, int size, void *msg);
extern int  Mbox_CondSend(int mbox, int size, void *msg);
extern int  Mbox_CondReceive(int mbox, int size, void *msg);

/* Phase 5 -- User Function Prototypes */
extern void *VmInit(int mappings, int pages, int frames, int pagers);
extern void VmCleanup(void);

#endif
