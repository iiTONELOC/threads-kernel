#pragma once
#ifndef _Devices_H
#define _Devices_H
#include <THREADSLib.h>
#include <SystemCalls.h>
#include <DoubleSeaLib.h>
#include "Devices.h"

enum DEVICE_PROC_STATUS
{
    DEVICE_PROC_INVALID = -1,
    DEVICE_PROC_FREE, // 0
    DEVICE_PROC_READY,
    DEVICE_PROC_IN_USE,
    DEVICE_PROC_WAITING_IO_READ,
    DEVICE_PROC_WAITING_IO_WRITE,
    DEVICE_PROC_WAITING_IO_INFO,
    DEVICE_PROC_BLOCKED,
    DEVICE_PROC_SLEEPING,                                  // leave this as the last status
    DEVICE_PROC_STATUS_MAX = DEVICE_PROC_SLEEPING,         // max valid status value
    DEVICE_PROC_STATUS_COUNT = DEVICE_PROC_STATUS_MAX + 1, // total number of valid statuses

};

enum TDISK_MODE
{
    TDISK_INVALID = -1,
    TDISK_UNINITIALIZED = 0,
    TDISK_READ = 4,
    TDISK_WRITE = 8,

};

enum SUPPORTED_ALGORITHMS
{
    TDISK_FCFS = 0,          // First Come First Served
    TDISK_SSTF = 1,          // Shortest Seek Time First
    TDISK_ELEVATOR = 2,      // Elevator Algorithm
    TDISK_ONE_DIRECTION = 3, // One Direction
};

#ifndef TDISK_ALGO
#define TDISK_ALGO TDISK_FCFS // default algorithm
#endif

/* -------------------------------- Typedefs and Structs ------------------------------- */

typedef struct io_request
{

    int forPid;                // process id of the requesting process
    int startTrack;            // start track
    int numSectors;            // number of sectors to read/write
    int startSector;           // start sector
    int startPlatter;          // start platter
    char *deviceName;          // name of the device
    char *readBuffer;          // buffer to read data into
    char *writeBuffer;         // buffer to write data from
    int opResultStatus;        // operation status
    enum TDISK_MODE mode;      // read or write
    int numSectorsCompleted;   // number of sectors completed
                               /* Waiting list usage for device disk
                                  Threads supports 2 disks
                               */
    struct io_request *pNext0; // pointer to the next IO request for disk 0
    struct io_request *pPrev0; // pointer to the previous IO request for disk 0
    struct io_request *pNext1; // pointer to the next IO request for disk 1
    struct io_request *pPrev1; // pointer to the previous IO request for disk 1
} IO_Request;

typedef struct device_proc
{
    int pid;
    int mutex;
    int status;
    size_t wakeTime;           // time when the process should wake up
    IO_Request *ioRequest;     // pointer to the IO request
                               // reserved for the sleep list
    struct device_proc *pNext; // pointer to the next device process in the list
    struct device_proc *pPrev; // pointer to the previous device process in the list

} DevicesProcess;

typedef struct
{
    int pid; // process id of the disk driver
    int mutex;
    int tracks;
    int platters;
    int currentTrack;           // current track being processed
    int currentSector;          // current sector being processed
    int currentPlatter;         // current platter being processed
    IO_Request *currentRequest; // current request being processed
    DSL_List requestList;
    char deviceName[THREADS_MAX_DEVICE_NAME];
} DiskInformation;

union DiskInfoResult
{
    struct
    {
        uint16_t trackCount;  // number of tracks on the disk (bits 0-15)
        uint8_t platterCount; // number of platters on the disk (bits 16-23)
        uint8_t resultCode;   // result code from the disk driver (bits 24-31)
    } info;
    uint32_t rawResult; // raw 32-bit result
};

union IO_RequestResult
{
    struct
    {
        unsigned int notUsed : 24;
        uint8_t resultCode; // result code from the disk driver (bits 24-31)
    } info;
    uint32_t rawResult; // raw 32-bit result
};

typedef device_control_block_t DeviceControlBlock;

/* -- Put here from threads for reference

    THREADS_DISK_SECTOR_SIZE    512
    THREADS_DISK_SECTOR_COUNT   16   Sectors per track
    THREADS_DISK_MAX_PLATTERS   3
    THREADS_DISK_MAX_TRACKS     1024 Max number of track

*/

/* TODO:

    - system calls to implement
        - SYS_DISKREAD 11
        - SYS_DISKWRITE 12
        - SYS_DISKINFO 13 (SYS_DISKSIZE according to SystemCalls.h)

    - functions to implement (non system calls)
        - DiskDriver
        - DiskRead
        - DiskWrite
        - DiskInfo

*/

/* -------------------------------- Macros --------------------------------- */

#define SUPPORTED_SYS_CALL_END 13                                                        // inclusive index into vector table
#define SUPPORTED_SYS_CALL_START 10                                                      // inclusive index into vector table
#define SECONDS_IN_MILLISECOND 1000                                                      // 1000 milliseconds in a second
#define NUM_MILLISEC_IN_MICROSEC 1000                                                    // 1000 microseconds in a millisecond
#define OFFSETOF_DISK_0_NEXT offsetof(IO_Request, pNext0)                                // Offset to the pNext field in the IO_Request structure
#define OFFSETOF_DISK_1_NEXT offsetof(IO_Request, pNext1)                                // Offset to the pNext field in the IO_Request structure
#define OFFSETOF_SLEEP_NEXT offsetof(DevicesProcess, pNext)                              // Offset to the pNext field in the UserProcess structure
#define SUPPORTED_SYS_CALL_COUNT (SUPPORTED_SYS_CALL_END - SUPPORTED_SYS_CALL_START + 1) // number of supported system calls

#endif
/* _Devices_H */
