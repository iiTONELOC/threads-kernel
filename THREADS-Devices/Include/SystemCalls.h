#pragma once


#define MAXSEMS         200


#define SYS_TERMREAD		    1
#define SYS_TERMWRITE		    2
#define SYS_SPAWN		        3
#define SYS_WAIT		        4
#define SYS_EXIT		        5
#define SYS_MAILBOX_CREATE		6
#define SYS_MAILBOX_FREE		7
#define SYS_MAILBOX_SEND		8
#define SYS_MAILBOX_RECEIVE		9
#define SYS_SLEEP		        10
#define SYS_DISKREAD		    11
#define SYS_DISKWRITE		    12
#define SYS_DISKINFO		    13
#define SYS_SEMCREATE		    14
#define SYS_SEMP		        15
#define SYS_SEMV		        16
#define SYS_SEMFREE		        17
#define SYS_GETTIMEOFDAY	    18
#define SYS_CPUTIME		        19
#define SYS_GETPID		        20

void sys_exit(int resultCode);
int sys_wait(int* pStatus);
int k_semp(int sem_id);
int k_semv(int sem_id);
int k_semcreate(int initial_value);
int k_semfree(int sem_id);

int sys_spawn(char* name, int (*startFunc)(char*), char* arg, int stackSize, int priority);
