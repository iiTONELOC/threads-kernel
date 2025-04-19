#include "DeviceUtils.h"

/**
 * @brief Gets the elapsed time since the last clock interrupt.
 *
 * This function calculates the elapsed time in milliseconds since the last
 * clock interrupt was called.
 *
 * @return The elapsed time in milliseconds.
 */
size_t GetElapsedTime()
{
    static size_t elapsed = 0;  // time elapsed since last context switch
    static size_t lastTime = 0; // last time the clock interrupt was called
    uint32_t currentTime = system_clock();

    // calculate the time elapsed since the last time this function was called
    if (lastTime != 0)
    {
        // should be in μs
        elapsed += (currentTime - lastTime);
    }

    // update the last time the clock interrupt was called
    lastTime = currentTime;

    // return the elapsed time in milliseconds
    return (elapsed / NUM_MILLISEC_IN_MICROSEC);
}

/**
 * @brief Converts seconds to milliseconds.
 */
size_t ConvertSecondsToMilliseconds(int seconds)
{
    size_t milliseconds = 0;

    // use milliseconds per tick to convert seconds to milliseconds
    if (seconds > 0)
    {
        milliseconds = (size_t)(seconds * SECONDS_IN_MILLISECOND);
    }

    return milliseconds;
}

/**
 * @brief Initializes an IO_Request structure.
 *
 * @param ioRequest - Pointer to the IO_Request structure to initialize.
 */
void InitializeIoRequest(IO_Request *ioRequest)
{
    ioRequest->pNext0 = NULL;
    ioRequest->pPrev0 = NULL;
    ioRequest->pNext1 = NULL;
    ioRequest->pPrev1 = NULL;
    ioRequest->forPid = -1;
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
 * @brief Compares two IO_Request structures based on their startTrack values.
 *
 * @param request1 - Pointer to the first IO_Request structure.
 * @param request2 - Pointer to the second IO_Request structure.
 *
 * @return A negative value if request1 < request2, a positive value if request1 > request2,
 *         and 0 if they are equal.
 */
int compareIoRequest(void *request1, void *request2)
{
    if (request1 == NULL || request2 == NULL)
    {
        return 0;
    }
    IO_Request *req1 = (IO_Request *)request1;
    IO_Request *req2 = (IO_Request *)request2;

    if (req1 == NULL || req2 == NULL)
    {
        return 0;
    }

    return (req1->startTrack - req2->startTrack);
}

/**
 * @brief Compares two DevicesProcess structures based on their wakeTime values.
 *
 * @param request1 - Pointer to the first DevicesProcess structure.
 * @param request2 - Pointer to the second DevicesProcess structure.
 *
 * @return A negative value if request1 < request2, a positive value if request1 > request2,
 *         and 0 if they are equal.
 */
int compareSleepRequest(void *request1, void *request2)
{
    if (request1 == NULL || request2 == NULL)
    {
        return 0;
    }
    DevicesProcess *req1 = (DevicesProcess *)request1;
    DevicesProcess *req2 = (DevicesProcess *)request2;

    if (req1 == NULL || req2 == NULL)
    {
        return 0;
    }

    return (req1->wakeTime - req2->wakeTime);
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
    devicesProcess->wakeTime = 0;
    devicesProcess->ioRequest = NULL;
    devicesProcess->status = DEVICE_PROC_FREE;
    if (createMutex)
        devicesProcess->mutex = mailbox_create(1, sizeof(int)); // Create a mutex for the process
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
        InitializeDevicesProcess(&devicesProcessTable[i], TRUE);
    }
}
