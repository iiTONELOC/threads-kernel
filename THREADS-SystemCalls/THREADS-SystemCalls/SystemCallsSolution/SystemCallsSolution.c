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

void sys_exit(int resultCode);
int sys_wait(int* pStatus)
;

struct psr_bits {
    unsigned int cur_int_enable : 1;
    unsigned int cur_mode : 1;
    unsigned int prev_int_enable : 1;
    unsigned int prev_mode : 1;
    unsigned int unused : 28;
};

union psr_values {
    struct psr_bits bits;
    unsigned int integer_part;
};


/*****************************************************************************
   Name - checkKernelMode
   Purpose - Checks the PSR for kernel mode and halts if in user mode
   Parameters -
   Returns -
   Side Effects - Will halt if not in kernel mode
****************************************************************************/
static inline void checkKernelMode(const char* functionName)
{
    union psr_values psrValue;

    console_output(TRUE, "checkKernelMode(): verifying kernel mode for %d, %s\n", 1, functionName);

    psrValue.integer_part = get_psr();
    if (psrValue.bits.cur_mode == 0)
    {
        console_output(FALSE, "Kernel mode expected, but function called in user mode.\n");
        stop(1);
    }
}

/* -------------------- Typedefs and Structs ------------------------------- */
typedef struct sem_data
{
    int status;
    int sem_id;
    int count;
    int mboxMutex;
    TList waiting;
} SemData;

typedef struct user_proc
{
    struct user_proc* pNextSibling;
    struct user_proc* pPrevSibling;
    struct user_proc* pNextSem;
    struct user_proc* pPrevSem;
    TList             children;
    char             arg[MAXARG];       /* args passed to process */
    char             name[MAXNAME];     /* process's name */
    void* state;             /* current context for process */
    short            pid;               /* process id */
    struct user_proc* pParent;
    int              mboxMutex;
    int              mboxStartup;
    int              mboxSem;
    int              (*startFunc) (char*); /* function where process begins -- launch */
    int              semFreed;          /* used to be notified that a semphore was freed while waiting*/
} UserProcess;

#define USERMODE   set_psr(get_psr() & ~PSR_KERNEL_MODE)
#define KERNELMODE psr_set(psr_get() | PSR_CURRENT_MODE)

#define NODE_SIBLING_OFFSET    0
#define NODE_SEM_QUEUE_OFFSET  (sizeof(struct user_proc *) * 2)
#define STATUS_POSITON_EMPTY         0
#define STATUS_POSITON_INUSE         1
#define STATUS_POSITON_FREED         2

#define SEM_TABLE_INIT      1
#define SEM_TABLE_GET_ENTRY 2
#define SEM_TABLE_GET_NEW   3

#define MAXSEMS 200
/* -------------------------- Globals ------------------------------------- */

static UserProcess userProcTable[MAXPROC];
//static SemData semTable[MAXSEMS];
static int nextsem_id = 0;
static int semCount = 0;

/* ------------------------- Prototypes ----------------------------------- */
int MessagingEntryPoint(char*);
extern int SystemCallsEntryPoint(char*);
static void system_call_handler(system_call_arguments_t* args);


static int launchUserProcess(char* pArg);
static SemData* SemTable(int action, int* index);

int sys_spawn(char* name, int (*startFunc)(char*), char* arg, int stackSize, int priority);

int MessagingEntryPoint(char* arg)
{
    int	status = -1;
    int pid;

    checkKernelMode(__func__);

    //    unsigned long seed = mix(clock(), time(NULL), getpid());

    /* Not necessary, but showing we initialized these */
    SemTable(SEM_TABLE_INIT, 0);

    /* initialize the system call vector */
    for (int i = 0; i < THREADS_MAX_SYSCALLS; i++)
    {
        systemCallVector[i] = system_call_handler;
    }

    /* create a mailbox for each process entry */
    for (int i = 0; i < MAXPROC; ++i)
    {
        userProcTable[i].mboxStartup = mailbox_create(1, sizeof(int));
        userProcTable[i].mboxSem = mailbox_create(0, sizeof(int));
        userProcTable[i].mboxMutex = mailbox_create(1, sizeof(int));
        TListInitialize(&userProcTable[i].children, NODE_SIBLING_OFFSET, NULL);
    }

#ifdef SYSTEM_CALLS_PROJECT
    pid = sys_spawn("SystemCalls", SystemCallsEntryPoint, NULL, THREADS_MIN_STACK_SIZE * 4, 3);
    sys_wait(&status);
#else
    pid = k_spawn("SystemCalls", start3, NULL, 4 * USLOSS_MIN_STACK, 3);

    if (pid >= 0)
    {
        pid = join(&status);
    }
    else
    {
        console_output(FALSE, "MessagingEntryPoint(): unable to k_spawn SystemCalls process\n");
    }

    console_output(FALSE, "MessagingEntryPoint(): join returned pid = %d, status = %d\n",
        pid, status);

#endif /* SYSTEM_CALLS_PROJECT */

    return 0;
} /* MessagingEntryPoint */

// attempt 1 to obfuscate the sem table
static SemData* SemTable(int action, int* pIndex)
{
    int newsem_id = -1;
    int position = nextsem_id % MAXSEMS;
    int count = 0;

    SemData* pEntry = NULL;
#define semTableArray z5v03TteYq 
    static SemData z5v03TteYq[MAXSEMS];

    switch (action)
    {
    case SEM_TABLE_INIT:
        /* Initialize the semaphore table. */
        for (int i = 0; i < MAXSEMS; ++i)
        {
            semTableArray[i].status = STATUS_POSITON_EMPTY;
            TListInitialize(&semTableArray[i].waiting, NODE_SEM_QUEUE_OFFSET, NULL);
            semTableArray[i].mboxMutex = mailbox_create(1, sizeof(int));
        }
        break;
    case SEM_TABLE_GET_ENTRY:

        if (*pIndex < MAXSEMS && *pIndex >= 0)
        {
            pEntry = &semTableArray[*pIndex];
        }
        break;
    case SEM_TABLE_GET_NEW:

        if (semCount < MAXSEMS)
        {
            while (count < MAXSEMS && semTableArray[position].status != STATUS_POSITON_EMPTY)
            {
                nextsem_id++;
                count++;
                position = nextsem_id % MAXSEMS;
            }
            newsem_id = nextsem_id++;
        }

        *pIndex = newsem_id;
        if (newsem_id != -1)
        {
            pEntry = &semTableArray[newsem_id % MAXSEMS];
        }
        break;
    default:
        break;
    }
    return pEntry;
}


static int launchUserProcess(char* pArg)
{
    int pid;
    int procSlot;
    int resultCode;
    UserProcess* pProc;

    pid = k_getpid();
    procSlot = pid % MAXPROC;
    pProc = &userProcTable[procSlot];

    /* wait for the initialize process to complete. */
    mailbox_receive(pProc->mboxStartup, NULL, 0, TRUE);

    // if signaled when in the sys handler, then Exit
    if (signaled())
    {
        console_output(FALSE, "%s - Process signaled in launch.\n", "launchUserProcess");
        sys_exit(0);
    }

    set_psr(get_psr() & ~PSR_KERNEL_MODE);

    resultCode = pProc->startFunc(pProc->arg);

    Exit(resultCode);

    return 0;
}

int sys_getpid(int* pPid)
{
    *pPid = k_getpid();

    return 0;
}

int k_semp(int sem_id)
{
    SemData* pSem;
    int pid = k_getpid();
    int semIndex;
    int result = -1;

    semIndex = sem_id % MAXSEMS;
    pSem = SemTable(SEM_TABLE_GET_ENTRY, &semIndex);
    if (pSem != NULL)
    {
        if (pSem->status == STATUS_POSITON_INUSE && pSem->sem_id == sem_id)
        {
            // success, will Exit if freed
            result = 0;

            /* get mutex access to this semaphore */
            mailbox_send(pSem->mboxMutex, NULL, 0, TRUE);
            if (pSem->count == 0)
            {
                // add self to queue and block
                TListAddNode(&pSem->waiting, &userProcTable[pid % MAXPROC]);

                /* Release the mutex */
                mailbox_receive(pSem->mboxMutex, NULL, 0, TRUE);

                /* Wait for the signal */
                mailbox_receive(userProcTable[pid % MAXPROC].mboxSem, NULL, 0, TRUE);

                /* regain mutex access to this semaphore */
                mailbox_send(pSem->mboxMutex, NULL, 0, TRUE);

                // check to see if the semaphore was freed while we were waiting
                if (userProcTable[pid % MAXPROC].semFreed)
                {
                    /* Release the mutex */
                    mailbox_receive(pSem->mboxMutex, NULL, 0, TRUE);

                    /* Exit */
                    sys_exit(1);
                }
            }
            else
            {
                --pSem->count;
            }

            /* Release the mutex */
            mailbox_receive(pSem->mboxMutex, NULL, 0, TRUE);
        }
    }
    return result;
}

int k_semv(int sem_id)
{
    SemData* pSem;
    int result = -1;
    UserProcess* pWaitingProc;
    int semIndex;

    semIndex = sem_id % MAXSEMS;
    pSem = SemTable(SEM_TABLE_GET_ENTRY, &semIndex);
    if (pSem != NULL)
    {
        if (pSem->status == STATUS_POSITON_INUSE && pSem->sem_id == sem_id)
        {
            // success 
            result = 0;

            /* get mutex access to this semaphore */
            /* keep the mutex until we finish processing the list. */
            mailbox_send(pSem->mboxMutex, NULL, 0, TRUE);

            if (pSem->waiting.count > 0)
            {
                pWaitingProc = TListPopNode(&pSem->waiting);

                /* Release the mutex */
                mailbox_receive(pSem->mboxMutex, NULL, 0, TRUE);

                /* signal the waiting process*/
                mailbox_send(pWaitingProc->mboxSem, NULL, 0, FALSE);

            }
            else
            {
                pSem->count++;
                /* Release the mutex */
                mailbox_receive(pSem->mboxMutex, NULL, 0, TRUE);
            }
        }
    }
    return result;
}

int k_semcreate(int initial_value)
{
    int sem_id = -1;
    SemData* pSem;

    if (initial_value >= 0)
    {
        pSem = SemTable(SEM_TABLE_GET_NEW, &sem_id);
        if (pSem != NULL)
        {
            pSem->status = STATUS_POSITON_INUSE;
            pSem->count = initial_value;
            pSem->sem_id = sem_id;
            ++semCount;
        }

    }
    return sem_id;
}

int k_semfree(int sem_id)
{
    int result = -1;
    SemData* pSem;
    int semIndex;

    semIndex = sem_id % MAXSEMS;
    pSem = SemTable(SEM_TABLE_GET_ENTRY, &semIndex);

    if (pSem != NULL)
    {
        if (pSem != NULL && pSem->sem_id == sem_id)
        {
            result = 0;
            UserProcess* pProcess;

            // Exit all of the waiting processes
            if (pSem->waiting.count > 0)
            {
                result = 1;
                while ((pProcess = TListPopNode(&pSem->waiting)) != NULL)
                {
                    pProcess->semFreed = 1;
                    mailbox_send(pProcess->mboxSem, NULL, 0, FALSE);
                }
            }
            pSem->status = STATUS_POSITON_EMPTY;
            --semCount;

        }
    }
    else
    {
        sem_id = -1;
    }

    return result;
}

int sys_spawn(char* name, int (*startFunc)(char*), char* arg, int stackSize, int priority)
{
    int parentPid;
    int pid = -1;
    UserProcess* pProcess;

    if (startFunc == NULL)
    {
        console_output(FALSE, "Start function parameter is invalid.");
    }
    else if (name == NULL || strlen(name) > MAXNAME)
    {
        console_output(FALSE, "Process name parameter is invalid.");
    }
    else if (stackSize < THREADS_MIN_STACK_SIZE)
    {
        console_output(FALSE, "Process stack size is invalid.");
    }
    else if (priority > HIGHEST_PRIORITY || priority < LOWEST_PRIORITY)
    {
        console_output(FALSE, "Process priority is invalid.");
    }
    else
    {
        parentPid = k_getpid();

        // create the new process
        pid = k_spawn(name, launchUserProcess, arg, stackSize, priority);
        if (pid < 0)
        {
            console_output(FALSE, "Failed to create user process.");
        }
        else
        {
            // get a pointer to the proc table entry to fill in
            pProcess = &userProcTable[pid % MAXPROC];
            strncpy(pProcess->name, name, MAXNAME - 1);
            if (arg != NULL)
            {
                strncpy(pProcess->arg, arg, MAXARG - 1);
            }
            pProcess->startFunc = startFunc;
            pProcess->pid = pid;
            pProcess->pParent = &userProcTable[parentPid % MAXPROC];   // save a pointer to the parent proc
            pProcess->semFreed = 0;

            TListAddNode(&pProcess->pParent->children, pProcess);         // add self to parent's list of children

            mailbox_send(pProcess->mboxStartup, NULL, 0, FALSE);

        }
    }
    return pid;
}

int sys_gettimeofday(int* pTime)
{
    *pTime = system_clock();
    return 0;
}

int sys_cputime(int* pTime)
{
    *pTime = read_time();
    return 0;
}

void sys_exit(int resultCode)
{
    int pid;
    int procSlot;
    int exitCode = 0;
    UserProcess* pProcess;
    UserProcess* pChild;

    pid = k_getpid();
    procSlot = pid % MAXPROC;
    pProcess = &userProcTable[procSlot];

    mailbox_send(pProcess->mboxMutex, NULL, 0, TRUE);
    /* join with all children. */
    while ((pChild = TListPopNode(&pProcess->children)) != NULL)
    {
        mailbox_receive(pProcess->mboxMutex, NULL, 0, TRUE);
        //        k_kill(pChild->pid, SIG_TERM);
        k_wait(&exitCode);
        mailbox_send(pProcess->mboxMutex, NULL, 0, TRUE);
    }

    // remove this process from the list of children
    if (pProcess->pParent != NULL)
    {
        // remove this process from the list of children
        TListRemoveNode(&pProcess->pParent->children, pProcess);
    }

    mailbox_receive(pProcess->mboxMutex, NULL, 0, TRUE);

    /* quit */
    k_exit(resultCode);
}

int sys_wait(int* pStatus)
{
    int childPid;
    UserProcess* pProcess;

    childPid = k_wait(pStatus);

    pProcess = &userProcTable[childPid % MAXPROC];
    mailbox_send(pProcess->mboxMutex, NULL, 0, TRUE);
    if (pProcess->pParent != NULL)
    {
        // remove this process from the list of children
        TListRemoveNode(&pProcess->pParent->children, pProcess);
    }
    mailbox_receive(pProcess->mboxMutex, NULL, 0, TRUE);

    return childPid;
}
/* an error method to handle invalid system_calls */
static void sysNull(system_call_arguments_t* args)
{
    console_output(FALSE, "nullsys3(): Invalid system_call %d\n", args->call_id);
    console_output(FALSE, "nullsys3(): process %d terminating\n", k_getpid());
    sys_exit(1);
} /* nullsys3 */

static void system_call_handler(system_call_arguments_t* args)
{
    char* pFunctionName;
   

    if (args == NULL)
    {
        console_output(FALSE, "system_call(): Invalid system_call %d, no arguments.\n", args->call_id);
        console_output(FALSE, "system_call(): process %d terminating\n", k_getpid());
        sys_exit(1);
    }

    switch (args->call_id)
    {
    case SYS_SPAWN:
        pFunctionName = "Spawn";
        args->arguments[0] = (intptr_t)sys_spawn((char*)args->arguments[4], // name
            (int (*)(char*))args->arguments[0],  // start function
            (char*)args->arguments[1],           // arguments
            (int)args->arguments[2],        // stack size
            (int)args->arguments[3]);       // priority
        args->arguments[3] = (intptr_t)((intptr_t)args->arguments[0] >= 0 ? 0 : -1);
        break;
    case SYS_WAIT:
        pFunctionName = "Wait";
        args->arguments[0] = (intptr_t)sys_wait((int*)&args->arguments[1]);
        args->arguments[3] = (intptr_t)((intptr_t)args->arguments[0] >= 0 ? 0 : -1);
        break;
    case SYS_EXIT:
        pFunctionName = "Exit";
        sys_exit((int)args->arguments[0]);
        break;
    case SYS_SEMCREATE:
        pFunctionName = "SemCreate";
        args->arguments[0] = (intptr_t)k_semcreate((int)args->arguments[0]);
        args->arguments[3] = (intptr_t)((intptr_t)args->arguments[0] >= 0 ? 0 : -1);
        break;
    case SYS_SEMFREE:
        pFunctionName = "SemFree";
        args->arguments[3] = (intptr_t)k_semfree((int)args->arguments[0]);
        break;
    case SYS_SEMV:
        pFunctionName = "SemV";
        args->arguments[3] = (intptr_t)k_semv((int)args->arguments[0]);
        break;
    case SYS_SEMP:
        args->arguments[3] = (intptr_t)k_semp((int)args->arguments[0]);
        pFunctionName = "SemP";
        break;
    case SYS_GETPID:
        pFunctionName = "GetPID";
        sys_getpid((int*)&args->arguments[0]);
        break;
    case SYS_GETTIMEOFDAY:
        sys_gettimeofday((int*)&args->arguments[0]);
        pFunctionName = "GetTimeOfDay";
        break;
    case SYS_CPUTIME:
        pFunctionName = "CPUTime";
        sys_cputime((int*)&args->arguments[0]);
        break;
    default:
        pFunctionName = "null";
        sysNull(args);
        break;
    }

    // if signaled when in the sys handler, then Exit
    if (signaled())
    {
        console_output(FALSE, "%s - Process signaled while in system call.\n", pFunctionName);
        sys_exit(0);
    }

    USERMODE;
}
