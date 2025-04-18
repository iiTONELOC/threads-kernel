#pragma once
#ifndef _Devices_H
#define _Devices_H
#include <SystemCalls.h>
#include <DoubleSeaLib.h>
#include "Devices.h"

/* -------------------------------- Typedefs and Structs ------------------------------- */

enum DEVICE_PROC_STATUS
{
    DEVICE_PROC_FREE = 0,
    DEVICE_PROC_READY = 1,
    DEVICE_PROC_IN_USE = 2,
    DEVICE_PROC_WAITING = 3,
    DEVICE_PROC_INVALID = -1,
    DEVICE_PROC_STATUS_COUNT = 4
};

/* -------------------------------- Typedefs and Structs ------------------------------- */

typedef struct device_proc
{
    int pid;
    int status;
    int priority;
    char *readBuffer;
    char *writeBuffer;
    /* Waiting list usage for device disk
       Threads supports 2 disks
    */
    struct device_proc *pNext0;
    struct device_proc *pPrev0;
    struct device_proc *pNext1;
    struct device_proc *pPrev2;

} DevicesProcess;

typedef struct
{
    int mutex;
    int tracks;
    int platters;
    DSL_List waitingProcs;
    char deviceName[THREADS_MAX_DEVICE_NAME];
} DiskInformation;

/* -- Put here from threads for reference

    THREADS_DISK_SECTOR_SIZE    512
    THREADS_DISK_SECTOR_COUNT   16   Sectors per track
    THREADS_DISK_MAX_PLATTERS   3
    THREADS_DISK_MAX_TRACKS     256    Max number of track

*/

/* TODO:

    - system calls to implement
        - SYS_SLEEP 10
        - SYS_DISKREAD 11
        - SYS_DISKWRITE 12
        - SYS_DISKINFO 13 (SYS_DISKSIZE according to SystemCalls.h)

    - functions to implement (non system calls)
        - SleepSeconds
        - DiskDriver
        - ClockDriver
        - DiskRead
        - DiskWrite
        - DiskInfo

*/

union DiskInfoResult
{
    struct
    {
        uint8_t trackCount;   // number of tracks on the disk (bits 0-7)
        uint8_t platterCount; // number of platters on the disk (bits 8-15)
        uint8_t reserved;     // reserved for alignment or future use (bits 16-23)
        uint8_t resultCode;   // result code from the disk driver (bits 24-31)
    } info;
    uint32_t rawResult; // raw 32-bit result
};

typedef device_control_block_t DeviceControl;
#define DISC_TRACK_MASK 0x000000FF                                                       // access bits 0-7
#define DISC_PLATTER_MASK 0x0000FF00                                                     // access bits 8-15
#define DISC_RESULT_MASK 0xFF000000                                                      // access bits 31-24
#define SUPPORTED_SYS_CALL_END 13                                                        // inclusive index into vector table
#define SUPPORTED_SYS_CALL_START 10                                                      // inclusive index into vector table
#define SUPPORTED_SYS_CALL_COUNT (SUPPORTED_SYS_CALL_END - SUPPORTED_SYS_CALL_START + 1) // number of supported system calls
#define OFFSETOF_DISK_0_NEXT offsetof(DevicesProcess, pNext0)                            // Offset to the pNext field in the UserProcess structure
#define OFFSETOF_DISK_1_NEXT offsetof(DevicesProcess, pNext1)                            // Offset to the pNext field in the UserProcess structure
#endif
/* _Devices_H */
