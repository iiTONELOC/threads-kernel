#pragma once
#ifndef _DEVICE_UTILS_H
#define _DEVICE_UTILS_H
#include <stdbool.h>
#include <_Devices.h>
#include <Messaging.h>

/**
 * @brief Gets the elapsed time since the last clock interrupt.
 *
 * This function calculates the elapsed time in milliseconds since the last
 * clock interrupt was called.
 *
 * @return The elapsed time in milliseconds.
 */
size_t GetElapsedTime();

/**
 * @brief Converts seconds to milliseconds.
 */
size_t ConvertSecondsToMilliseconds(int seconds);

/**
 * @brief Initializes an IO_Request structure.
 *
 * @param ioRequest - Pointer to the IO_Request structure to initialize.
 */
void InitializeIoRequest(IO_Request *ioRequest);

/**
 * @brief Gets the unit number from the system call arguments.
 *
 * @param arg - Pointer to the system call arguments.
 * @return The unit number extracted from the arguments.
 */
int GetUnitFromArgs(system_call_arguments_t *arg);

/**
 * @brief Compares two IO_Request structures based on their startTrack values.
 *
 * @param request1 - Pointer to the first IO_Request structure.
 * @param request2 - Pointer to the second IO_Request structure.
 *
 * @return A negative value if request1 < request2, a positive value if request1 > request2,
 *         and 0 if they are equal.
 */
int compareIoRequest(void *request1, void *request2);

/**
 * @brief Compares two DevicesProcess structures based on their wakeTime values.
 *
 * @param request1 - Pointer to the first DevicesProcess structure.
 * @param request2 - Pointer to the second DevicesProcess structure.
 *
 * @return A negative value if request1 < request2, a positive value if request1 > request2,
 *         and 0 if they are equal.
 */
int compareSleepRequest(void *request1, void *request2);

/**
 * @brief Initializes a DevicesProcess structure.
 *
 * @param devicesProcess - Pointer to the DevicesProcess structure to initialize.
 * @param createMutex - Flag indicating whether to create a mutex for the process.
 */
void InitializeDevicesProcess(DevicesProcess *devicesProcess, bool createMutex);

/**
 * @brief Initializes the device process table and pending I/O request table.
 *
 * @param devicesProcessTable - Pointer to the device process table.
 * @param pendingIoRequestTable - Pointer to the pending I/O request table.
 */
void InitializeTables(DevicesProcess *devicesProcessTable, IO_Request *pendingIoRequestTable);

/**
 * * @brief Sets the IO_Request structure with the provided arguments.
 */
void SetIoRequest(IO_Request *ioRequest, char *onDevice, int forPid, system_call_arguments_t *args, enum TDISK_MODE mode);
#endif