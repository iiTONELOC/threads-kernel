#pragma once

#ifdef BUILD_DLL
#define SCHEDULER_LIB_SPEC __declspec(dllexport) 
#else
#define SCHEDULER_LIB_SPEC
#endif


#define LOWEST_PRIORITY   0
#define HIGHEST_PRIORITY  5

#define MAXNAME           256
#define MAXARG            256
#define MAXPROC           50

/* Kill signals */
#define SIG_TERM			15

typedef int (*check_io_function) ();
SCHEDULER_LIB_SPEC check_io_function check_io;

SCHEDULER_LIB_SPEC int bootstrap(void* pArgs);


/* Functions that will become system calls. */
SCHEDULER_LIB_SPEC int  k_spawn(char* name, int(*entryPoint)(void*), void* arg, int stacksize, int priority);

SCHEDULER_LIB_SPEC void SchedulerSetEntryPoint(int(*entryPoint)(void*));

SCHEDULER_LIB_SPEC int   k_wait(int* pChildExitCode);
SCHEDULER_LIB_SPEC int   k_join(int pid, int* pChildExitCode);
SCHEDULER_LIB_SPEC int   k_kill(int pid, int signal);
SCHEDULER_LIB_SPEC void  k_exit(int exitCode);
SCHEDULER_LIB_SPEC int	  k_getpid(void);

/* Additional kernel-only functions. */
SCHEDULER_LIB_SPEC int   signaled(void);
SCHEDULER_LIB_SPEC void  display_process_table(void);
SCHEDULER_LIB_SPEC int   block(int block_status);
SCHEDULER_LIB_SPEC int   unblock(int pid);
SCHEDULER_LIB_SPEC int   get_start_time(void);
SCHEDULER_LIB_SPEC void  time_slice(void);
SCHEDULER_LIB_SPEC void  dispatcher();
SCHEDULER_LIB_SPEC int   read_time(void);
SCHEDULER_LIB_SPEC DWORD read_clock(void);
SCHEDULER_LIB_SPEC void enableInterrupts();
SCHEDULER_LIB_SPEC void disableInterrupts();

