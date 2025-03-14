#pragma once
#include "THREADSLib.h"

#define LOWEST_PRIORITY 0
#define HIGHEST_PRIORITY 5

#define MAXNAME 256
#define MAXARG 256
#define MAXPROC 50

/* Kill signals */
#define SIG_TERM 15

int bootstrap(void *pArgs);

typedef int (*check_io_function)();
extern check_io_function check_io;

/* Functions that will become system calls. */

/**
 * @brief Spawns a new process.
 *
 * Finds an empty entry in the process table and initializes information about
 * the process. Updates information in the parent to reflect child process
 * creation.
 *
 * @param name The name of the process
 * @param entryPoint The entry point function of the process
 * @param arg The arguments to pass to the process
 * @param stacksize The size of the stack
 * @param priority The priority of the process
 */
int k_spawn(char *name, int (*entryPoint)(void *), void *arg, int stacksize, int priority);

#ifdef BUILD_DLL
__declspec(dllexport) void SchedulerSetEntryPoint(int (*entryPoint)(void *));
#endif

/**
 * @brief Waits for a child process to exit.
 *
 * Waits for a child process to quit. Returns right away if no children
 * to wait for
 *
 * @param pChildExitCode The exit code of the child process
 *
 * @return The pid of the quitting child, or
 *       - 4 if the process has no children
 *       - 5 if the process was signaled in the join
 */
int k_wait(int *pChildExitCode);

/**
 * @brief Joins a non-parent process.
 *
 * Waits for the specified process to terminate and retrieves its exit code
 *
 * @param pid The process id to join
 * @param pChildExitCode A pointer to the exit code of the child process
 *
 * @return  0 on success, -5 if signaled in the join, or -1 if the process
 * could not be joined. Any attempt to join a process that is a parent or one
 * that does not exit will cause the kernel to HALT
 */
int k_join(int pid, int *pChildExitCode);

/**
 * @brief Signals a process with the specified signal
 *  This function sends the specified signal to the process indicated by the PID
 *  Currently SIG_TERM (15), is the only supported signal. Once a process has been
 *  signaled and future calls should return 1.
 *
 * @param pid The process id to signal
 * @param signal The signal to send
 *
 * @return 0 on success
 */
int k_kill(int pid, int signal);

/**
 * @brief Exits the current process.
 *
 * Exits a process and coordinates with the parent for cleanup and return of
 * the exit code.
 *
 * @param exitCode The code to return to the grieving parent
 */
void k_exit(int exitCode);

/**
 * @brief Returns the process ID for the currently running process
 */
int k_getpid(void);

/* Additional kernel-only functions. */

/**
 * @brief Checks if the calling process has been signaled
 *
 * @return 1 if the process has been signaled, 0 otherwise
 */
int signaled(void);

/**
 * @brief Prints the process table to the console
 */
void display_process_table(void);

/**
 * @brief Blocks the calling process with the specified status
 *
 * @param block_status The status to set the process to, must be greater than 10
 *
 * @return 0 if success, -5 if signaled while blocked
 */
int block(int block_status);

/**
 * @brief Unblocks the process with the specified pid
 *
 * @param pid The process id to unblock
 *
 * @return 0 if success, -1 if the process is not blocked or not valid
 */
int unblock(int pid);

/**
 * @brief Returns the start time for the calling process
 *
 */
int get_start_time(void);

/**
 * @brief Time slice function
 *
 * The time_slice function determines if the currently active process has exceeded its
 * current time slice. If the quantum value has been exceeded the dispatcher is called
 */
void time_slice(void);

/**
 * @brief Workhorse of the scheduler
 *
 * This function determines which process to run next and initiates a context switch if
 * necessary.
 */
void dispatcher();

/**
 * @brief Reads the current CPU time for the calling process, in milliseconds
 */
int read_time(void);

/**
 * @brief Reads the current system clock, value is in microseconds
 */
DWORD read_clock(void);
