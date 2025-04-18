#include "DeviceUtils.h"

/**
 * @brief Initializes an IO_Request structure.
 *
 * @param ioRequest - Pointer to the IO_Request structure to initialize.
 */
void InitializeIoRequest(IO_Request *ioRequest)
{
    ioRequest->pNext0 = NULL; // Initialize the next pointer for disk 0
    ioRequest->pPrev0 = NULL; // Initialize the previous pointer for disk 0
    ioRequest->pNext1 = NULL; // Initialize the next pointer for disk 1
    ioRequest->pPrev1 = NULL; // Initialize the previous pointer for disk 1
    ioRequest->forPid = -1;   // Initialize the process ID to -1
    ioRequest->startTrack = -1;
    ioRequest->numSectors = -1;
    ioRequest->startSector = -1;
    ioRequest->startPlatter = -1;
    ioRequest->deviceName = NULL;
    ioRequest->readBuffer = NULL;
    ioRequest->writeBuffer = NULL;
    ioRequest->opResultStatus = -1;
    ioRequest->direction = TDISK_UNINITIALIZED;
}

/**
 * @brief Initializes a DevicesProcess structure.
 *
 * @param devicesProcess - Pointer to the DevicesProcess structure to initialize.
 * @param createMutex - Flag indicating whether to create a mutex for the process
 *                       if not set, the mutex is left as is.
 */
void InitializeDevicesProcess(DevicesProcess *devicesProcess, bool createMutex)
{
    devicesProcess->pid = -1; // Initialize the process table
    devicesProcess->pNext = NULL;
    devicesProcess->pPrev = NULL;
    devicesProcess->sleepTime = 0;
    devicesProcess->ioRequest = NULL;
    devicesProcess->status = DEVICE_PROC_FREE;
    if (createMutex)
        devicesProcess->mutex = k_semcreate(0);
}

/**
 * @brief Initializes the device process table and pending I/O request table.
 *
 * @param devicesProcessTable - Pointer to the device process table.
 * @param pendingIoRequestTable - Pointer to the pending I/O request table.
 */
void InitializeTables(DevicesProcess *devicesProcessTable, IO_Request *pendingIoRequestTable)
{
    /* Initialize the process table */
    for (int i = 0; i < MAX_PROCESSES; ++i)
    {
        InitializeIoRequest(&pendingIoRequestTable[i]);
        InitializeDevicesProcess(&devicesProcessTable[i], true);
    }
}
