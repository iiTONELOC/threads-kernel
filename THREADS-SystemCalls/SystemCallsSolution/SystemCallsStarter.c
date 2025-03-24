#define SYSTEM_CALLS_PROJECT
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <THREADSLib.h>
#include <Messaging.h>
#include <Scheduler.h>
#include <TList.h>
#include "SystemCalls.h"
#include "libuser.h"

/* -------------------- Typedefs and Structs ------------------------------- */
typedef struct sem_data
{
    int status;
    int sem_id;
    int count;
    /* Add additional members needed. */
} SemData;

typedef struct user_proc
{
    struct user_proc* pNextSibling;
    struct user_proc* pPrevSibling;
    struct user_proc* pNextSem;
    struct user_proc* pPrevSem;
    TList             children;
    int              (*startFunc) (char*); /* function where process begins -- launch */
    /* Add additional members needed. */
} UserProcess;

#define MAXSEMS 200

/* -------------------------- Globals ------------------------------------- */
static UserProcess userProcTable[MAXPROC];
static SemData semTable[MAXSEMS];
static int nextsem_id = 0;

/* ------------------------- Prototypes ----------------------------------- */
void sys_exit(int resultCode);
int sys_wait(int* pStatus);
int MessagingEntryPoint(char*);
extern int SystemCallsEntryPoint(char*);
static void system_call_handler(system_call_arguments_t* args);
static int launchUserProcess(char* pArg);


int MessagingEntryPoint(char* arg)
{
    int	status = -1;
    int pid;

    checkKernelMode(__func__);


    /* Initialize the semaphore table */

    /* initialize the system call vector */

    /* Initialize the process table */

    pid = sys_spawn("SystemCalls", SystemCallsEntryPoint, NULL, THREADS_MIN_STACK_SIZE * 4, 3);
    sys_wait(&status);

    console_output(FALSE, "MessagingEntryPoint(): join returned pid = %d, status = %d\n",
        pid, status);

    return 0;
} /* MessagingEntryPoint */


static int launchUserProcess(char* pArg)
{
    int pid;
    int procSlot;
    int resultCode;
    UserProcess* pProc;

    /* wait for the initialize process to complete. */

    // if signaled when in the sys handler, then Exit
    if (signaled())
    {
        console_output(FALSE, "%s - Process signaled in launch.\n", "launchUserProcess");
        sys_exit(0);
    }

    /* Set mode to user mode */


    /* call the startup function for this process */


    /* Exit if the startup function returns */

    return 0;
}


int k_semp(int sem_id)
{
    int result = -1;
    return result;
}

int k_semv(int sem_id)
{
    int result = -1;
    return result;
}

int k_semcreate(int initial_value)
{
    int sem_id = -1;
    return sem_id;
}

int k_semfree(int sem_id)
{
    int result = -1;

    return result;
}

int sys_spawn(char* name, int (*startFunc)(char*), char* arg, int stackSize, int priority)
{
    int parentPid;
    int pid = -1;
    UserProcess* pProcess;

    /* validate the parameters */

    /* we are the parent*/
    parentPid = k_getpid();

    // create the new process
    pid = k_spawn(name, launchUserProcess, arg, stackSize, priority);
    if (pid < 0)
    {
        console_output(FALSE, "Failed to create user process.");
    }
    else
    {
        /* We now have the pid of the new process, what now? */

    }
    return pid;
}


static void spawn_syscall_handler(system_call_arguments_t* args)
{
    /* Handle the request */

    /* set mode to to usermode before returning.*/
}
