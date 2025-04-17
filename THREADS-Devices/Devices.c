#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <THREADSLib.h>
#include <Messaging.h>
#include <Scheduler.h>
#include <DoubleSeaLib.h>
#include <libuser.h>
#include <SystemCalls.h>

static int ClockDriver(char *);
static int DiskDriver(char *);

typedef struct devices_proc
{
    struct devices_proc *pNext;
    struct devices_proc *pPrev;
    int pid;
} DevicesProcess;

typedef struct
{
    int tracks;
    int platters;
    char deviceName[THREADS_MAX_DEVICE_NAME];
} DiskInformation;

static DevicesProcess DevicesProcesss[MAXPROC]; // Devices process table
static int running;                             /*semaphore to synchronize drivers and start3*/

static inline void checkKernelMode(const char *functionName);
extern int DevicesEntryPoint(char *);

int SystemCallsEntryPoint(char *arg)
{
    int i;
    int status;
    char buf[25];
    char name[128];
    int clockPID = 0;
    int diskPids[THREADS_MAX_DISKS];

    checkKernelMode(__func__);

    /* Assign system call handlers */

    /* Initialize the process table */
    for (int i = 0; i < MAXPROC; ++i)
    {
    }

    /*
     * Create clock device driver
     * Use a semaphore or mailbox for coordinating the start of the drive
     */
    // running = k_semcreate(0);
    clockPID = k_spawn("Clock driver", ClockDriver, NULL, THREADS_MIN_STACK_SIZE, HIGHEST_PRIORITY);
    if (clockPID < 0)
    {
        console_output(TRUE, "start3(): Can't create clock driver\n");
        stop(1);
    }

    /*
     * Wait for the clock driver to start. The idea is that ClockDriver
     * will V the semaphore "running" once it is running.
     */

    /*
     * Create the disk device drivers here.
     */
    for (i = 0; i < THREADS_MAX_DISKS; i++)
    {
        sprintf(buf, "%d", i);
        sprintf(name, "DiskDriver%d", i);
        diskPids[i] = k_spawn(name, DiskDriver, buf, THREADS_MIN_STACK_SIZE * 4, HIGHEST_PRIORITY);
        if (diskPids[i] < 0)
        {
            console_output(TRUE, "start3(): Can't create disk driver %d\n", i);
            stop(1);
        }
    }
    /* Wait for the disk drivers to start. */

    /*
     * Create first user-level process and wait for it to finish.
     */
    sys_spawn("DevicesEntryPoint", DevicesEntryPoint, NULL, 8 * THREADS_MIN_STACK_SIZE, 3);
    sys_wait(&status);

    /*
     * Terminate the device drivers cleanly
     */
    return 0;
}

static int ClockDriver(char *arg)
{
    DevicesProcess *pProc = NULL;
    int result;
    int status;

    /*
     * Let the parent know we are running and enable interrupts.
     */

    set_psr(get_psr() | PSR_INTERRUPTS);

    while (!signaled())
    {
        result = wait_device("clock", &status);
        if (result != 0)
        {
            return 0;
        }

        /*
         * Compute the current time and wake up any processes
         * whose time has come.
         */
    }
    return 0;
}

static int DiskDriver(char *arg)
{
    int unit = atoi(arg);
    DevicesProcess *pRequestingProc = NULL;
    DevicesProcess *pNextRequestingProc = NULL;
    int currentTrack = 0;
    device_control_block_t devRequest;

    set_psr(get_psr() | PSR_INTERRUPTS);

    /* Read the disk info */

    /* Operating loop */
    while (!signaled())
    {
    }
    return 0;
}

struct psr_bits
{
    unsigned int cur_int_enable : 1;
    unsigned int cur_mode : 1;
    unsigned int prev_int_enable : 1;
    unsigned int prev_mode : 1;
    unsigned int unused : 28;
};

union psr_values
{
    struct psr_bits bits;
    unsigned int integer_part;
};

/*****************************************************************************
   Name - checkKernelMode
   Purpose - Checks the PSR for kernel mode and stops if in user mode
   Parameters -
   Returns -
   Side Effects - Will stop if not in kernel mode
****************************************************************************/
static inline void checkKernelMode(const char *functionName)
{
    union psr_values psrValue;

    //    console_output(TRUE, "checkKernelMode(): verifying kernel mode for %d, %s\n", 1, functionName);

    psrValue.integer_part = get_psr();
    if (psrValue.bits.cur_mode == 0)
    {
        console_output(FALSE, "Kernel mode expected, but function called in user mode.\n");
        stop(1);
    }
}
