#pragma once
#ifndef _DEVICE_UTILS_H
#define _DEVICE_UTILS_H
#include <stdbool.h>

#include <_Devices.h>

/**
 * @brief Initializes an IO_Request structure.
 *
 * @param ioRequest - Pointer to the IO_Request structure to initialize.
 */
void InitializeIoRequest(IO_Request *ioRequest);

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

#endif