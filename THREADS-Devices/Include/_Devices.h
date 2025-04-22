#pragma once
#ifndef _Devices_H
#define _Devices_H
#include <THREADSLib.h>
#include <SystemCalls.h>
#include <DoubleSeaLib.h>
#include "Devices.h"

enum TDISK_MODE
{
    TDISK_INVALID = -1,
    TDISK_UNINITIALIZED = 0,
    TDISK_READ = 4,
    TDISK_WRITE = 8,

};

#ifndef TDISK_ALGO
#define TDISK_FCFS 0
#define TDISK_ALGO TDISK_FCFS // First Come First Served
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
    struct io_request *pNext0; // pointer to the next IO request for disk 0
    struct io_request *pPrev0; // pointer to the previous IO request for disk 0
    struct io_request *pNext1; // pointer to the next IO request for disk 1
    struct io_request *pPrev1; // pointer to the previous IO request for disk 1
} IO_Request;

typedef struct device_proc
{
    int pid;                   // process id of the device process
    int mutex;                 // mailbox id for mutual exclusion
    size_t wakeTime;           // time when the process should wake up
    IO_Request *ioRequest;     // pointer to the IO request
                               // reserved for the sleep list
    struct device_proc *pNext; // pointer to the next device process in the list
    struct device_proc *pPrev; // pointer to the previous device process in the list

} DevicesProcess;

typedef struct
{
    int pid;                    // process id of the disk driver
    int mutex;                  // mailbox id for mutual exclusion
    int index;                  // index of the disk driver
    int tracks;                 // number of tracks on the disk
    int platters;               // number of platters on the disk
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

union DiskIoResult
{
    struct
    {
        unsigned int notUsed : 24;
        uint8_t resultCode; // result code from the disk driver (bits 24-31)
    } info;
    uint32_t rawResult; // raw 32-bit result
};

typedef device_control_block_t DeviceControlBlock;

/* -------------------------------- Macros --------------------------------- */

#define SUPPORTED_SYS_CALL_END 13                                                        // inclusive index into vector table
#define SUPPORTED_SYS_CALL_START 10                                                      // inclusive index into vector table
#define SECONDS_IN_MILLISECOND 1000                                                      // 1000 milliseconds in a second
#define NUM_MILLISEC_IN_MICROSEC 1000                                                    // 1000 microseconds in a millisecond
#define OFFSETOF_DISK_0_NEXT offsetof(IO_Request, pNext0)                                // Offset to the pNext field in the IO_Request structure
#define OFFSETOF_DISK_1_NEXT offsetof(IO_Request, pNext1)                                // Offset to the pNext field in the IO_Request structure
#define OFFSETOF_SLEEP_NEXT offsetof(DevicesProcess, pNext)                              // Offset to the pNext field in the UserProcess structure
#define SUPPORTED_SYS_CALL_COUNT (SUPPORTED_SYS_CALL_END - SUPPORTED_SYS_CALL_START + 1) // number of supported system calls
#define SEEK_HANDLE_ERRS(diskInfo, pRequest, status)                            \
    if ((status = DiskSeek(diskInfo->index, pRequest->startTrack,               \
                           pRequest->startSector, pRequest->startPlatter)) < 0) \
    {                                                                           \
        console_output(FALSE,                                                   \
                       "DiskDriver()::Error seeking "                           \
                       "%s to track %d from track %d\n",                        \
                       diskInfo->deviceName, pRequest->startTrack,              \
                       diskInfo->currentTrack);                                 \
                                                                                \
        pRequest->opResultStatus = -1;                                          \
                                                                                \
        enableInterrupts();                                                     \
        mailbox_send(pRequest->forPid, NULL, 0, TRUE);                          \
        return;                                                                 \
    }

#endif
/* _Devices_H */
