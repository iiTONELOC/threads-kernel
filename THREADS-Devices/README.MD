# Device Drivers Project Solution

## CYBV 489 Principles of Operating Systems

`Anthony Tropeano` `Dan Tiger`

## Table of Contents

- [Overview](#device-drivers-project-scope)
- [Contributors](#contributors)
- [Usage](#usage)
- [Solution](#solution)

## Device Drivers Project Scope

The fourth stage of our OS development project extends your kernel with support for a clock driver and for disk input/output (I/O) operations. This functionality is exposed through four new system calls: `SleepSeconds`, `DiskRead`, `DiskWrite`, and `DiskInfo`.

To support these new system calls, the kernel introduces two types of internal driver processes: the `Clock Driver` and the `Disk Driver`. These driver processes run in kernel mode, with interrupts enabled, and handle key asynchronous behavior in the system.  

- The `Clock Driver` implements the behavior of Sleep.
- The `Disk Driver` is responsible for managing disk I/O requests.  

Each Disk Driver manages requests for its assigned disk independently, allowing multiple I/O operations to proceed concurrently across devices while still preserving correct sequencing and synchronization for individual disks.

## Contributors

| Name | College | Program | Contact |
| --- | --- | --- | --- |
| Anthony Tropeano | [**College of Applied Science and Technology**](https://azcast.arizona.edu/) | [*Cyber Operations/Cyber Engineering*](https://azcast.arizona.edu/academics/cyber-operations/cyber-engineering) | [**GitHub**](https://github.com/iiTONELOC) <br> [**Email**](mailto:atropeano@arizona.edu) |
| Dan Tiger | [**College of Applied Science and Technology**](https://azcast.arizona.edu/) | [*Applied Computing: Software Development*](https://azcast.arizona.edu/academics/applied-computing/software-development) | [**GitHub**](https://github.com/dtigergh) <br> [**Email**](mailto:dtiger@arizona.edu) |

## Usage

Part of the solution utilizes a Generic Linked List implementation, [DoubleSeaLib](https://github.com/iiTONELOC/DoubleSea) created by Anthony, as a DLL, and is excluded from the `threads-kernel` repo.

For simplicity, a ZIP folder has been provided which includes the necessary DLLs and Libs, but does not include any additional build files.

The solution folder needs to be opened in Visual Studio so the project can be build before the test cases are executed.

Once unzipped, the prebuilt solution and its tests can be executed directly using the Batch file included in the `bin/` folder titled: `RunSysCallTests.bat`:

```bash
cd <path_to_THREADS-SystemCalls/bin>

.\RunSysCallTests.bat

# expected output


DevicesTest00: Started
DevicesTest00:  Going to sleep for 10 seconds
DevicesTest00: Slept 10020
All processes completed.

DevicesTest01: started
DevicesTest01: Waiting on Child
DevicesTest01-Child1:	started
DevicesTest01-Child1:	Sleeping for 1 seconds
DevicesTest01-Child2:	started
DevicesTest01-Child2:	Sleeping for 3 seconds
DevicesTest01-Child3:	started
DevicesTest01-Child3:	Sleeping for 5 seconds
DevicesTest01-Child4:	started
DevicesTest01-Child4:	Sleeping for 7 seconds
DevicesTest01-Child5:	started
DevicesTest01-Child5:	Sleeping for 9 seconds
DevicesTest01-Child6:	started
DevicesTest01-Child6:	Sleeping for 11 seconds
DevicesTest01-Child7:	started
DevicesTest01-Child7:	Sleeping for 13 seconds
DevicesTest01-Child8:	started
DevicesTest01-Child8:	Sleeping for 15 seconds
DevicesTest01-Child9:	started
DevicesTest01-Child9:	Sleeping for 17 seconds
DevicesTest01-Child10:	started
DevicesTest01-Child10:	Sleeping for 19 seconds
DevicesTest01-Child1:	Slept 1072 milliseconds
DevicesTest01: Wait returned 0, pid:9, status 0
DevicesTest01: Waiting on Child
DevicesTest01-Child2:	Slept 3055 milliseconds
DevicesTest01: Wait returned 0, pid:10, status 0
DevicesTest01: Waiting on Child
DevicesTest01-Child3:	Slept 5050 milliseconds
DevicesTest01: Wait returned 0, pid:11, status 0
DevicesTest01: Waiting on Child
DevicesTest01-Child4:	Slept 7063 milliseconds
DevicesTest01: Wait returned 0, pid:12, status 0
DevicesTest01: Waiting on Child
DevicesTest01-Child5:	Slept 9060 milliseconds
DevicesTest01: Wait returned 0, pid:13, status 0
DevicesTest01: Waiting on Child
DevicesTest01-Child6:	Slept 11057 milliseconds
DevicesTest01: Wait returned 0, pid:14, status 0
DevicesTest01: Waiting on Child
DevicesTest01-Child7:	Slept 13034 milliseconds
DevicesTest01: Wait returned 0, pid:15, status 0
DevicesTest01: Waiting on Child
DevicesTest01-Child8:	Slept 15054 milliseconds
DevicesTest01: Wait returned 0, pid:16, status 0
DevicesTest01: Waiting on Child
DevicesTest01-Child9:	Slept 17021 milliseconds
DevicesTest01: Wait returned 0, pid:17, status 0
DevicesTest01: Waiting on Child
DevicesTest01-Child10:	Slept 19027 milliseconds
DevicesTest01: Wait returned 0, pid:18, status 0
All processes completed.

DevicesTest02: Started
DevicesTest02:  Calling DiskInfo for disk0
DevicesTest02:  DiskInfo returned sectorSize 512, sectorCount 16, trackCount 128, platterCount 1
DevicesTest02:  Calling DiskInfo for disk1
DevicesTest02:  DiskInfo returned sectorSize 512, sectorCount 16, trackCount 512, platterCount 3
All processes completed.

DevicesTest03:	Started
DevicesTest03:	Calling DiskInfo for disk0
DevicesTest03:	DiskInfo returned sectorSize 512, sectorCount 16, trackCount 128, platterCount 1
DevicesTest03:	Writing to disk 0, platter 0, track 5, sector 0
DevicesTest03:	DiskWrite returned status = 0
DevicesTest03:	Reading from disk 0, platter 0, track 5, sector 0
DevicesTest03:	DiskRead returned status = 0
DevicesTest03:	In and Out buffers match!
All processes completed.

DevicesTest04:	Started
DevicesTest04:	Calling DiskInfo for disk0
DevicesTest04:	DiskInfo returned sectorSize 512, sectorCount 16, trackCount 128, platterCount 1
DevicesTest04:	Writing to disk 0, platter 0, track 6, sectors 3-6
DevicesTest04:	DiskWrite returned status = 0
DevicesTest04:	Reading from disk 0, platter 0, track 6, sectors 3-6
DevicesTest04:	DiskRead returned status = 0
DevicesTest04:	In and Out buffers match!
All processes completed.

DevicesTest05:	Started
DevicesTest05:	Writing to disk 0, platter 0, track 1, sectors 1-3
DevicesTest05:	DiskWrite returned status = 0
DevicesTest05:	Writing to disk 1, platter 1, track 2, sectors 8-12
DevicesTest05:	DiskWrite returned status = 0
DevicesTest05:	Reading from disk 0, platter 0, track 6, sectors 3-6
DevicesTest05:	DiskRead returned status = 0
DevicesTest05:	In and Out buffers match!
DevicesTest05:	Reading from disk 1, platter 1, track 2, sectors 8-12
DevicesTest05:	DiskRead returned status = 0
DevicesTest05:	In and Out buffers match!
All processes completed.

DevicesTest06:	Started
DevicesTest06:	Writing data to 3 disk sectors, wrapping to next track
DevicesTest06:	DiskWrite returned status = 0
DevicesTest06:	Reading data from 3 disk sectors, wrapping to next track
DevicesTest06:	DiskRead returned status = 0
DevicesTest06:	In and Out buffers match!
All processes completed.
	DevicesTest07: started
DevicesTest07-Child1:	started
DevicesTest07-Child2:	started
DevicesTest07-Child3:	started
DevicesTest07-Child4:	started
DevicesTest07-Child1:	DiskWrite (disk0) -  platter 0, track 00, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest07-Child2:	DiskWrite (disk0) -  platter 0, track 05, sectorStart 05, sectorCount 01, status 0x00000000
DevicesTest07-Child3:	DiskWrite (disk0) -  platter 0, track 09, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest07-Child4:	DiskWrite (disk0) -  platter 0, track 02, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest07-Child1:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest07-Child2:	DiskWrite (disk0) -  platter 0, track 04, sectorStart 04, sectorCount 01, status 0x00000000
DevicesTest07-Child3:	DiskWrite (disk0) -  platter 0, track 09, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest07-Child4:	DiskWrite (disk0) -  platter 0, track 09, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest07-Child1:	DiskWrite (disk0) -  platter 0, track 00, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest07-Child2:	DiskWrite (disk0) -  platter 0, track 01, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest07-Child3:	DiskWrite (disk0) -  platter 0, track 02, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest07-Child4:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest07-Child1:	DiskWrite (disk0) -  platter 0, track 05, sectorStart 05, sectorCount 01, status 0x00000000
DevicesTest07-Child2:	DiskWrite (disk0) -  platter 0, track 06, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest07-Child3:	DiskWrite (disk0) -  platter 0, track 07, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest07-Child4:	DiskWrite (disk0) -  platter 0, track 08, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest07-Child1:	DiskWrite (disk0) -  platter 0, track 00, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest07-Child2:	DiskWrite (disk0) -  platter 0, track 01, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest07-Child3:	DiskWrite (disk0) -  platter 0, track 02, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest07-Child4:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest07-Child1:	DiskWrite (disk0) -  platter 0, track 05, sectorStart 05, sectorCount 01, status 0x00000000
DevicesTest07-Child2:	DiskWrite (disk0) -  platter 0, track 06, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest07-Child3:	DiskWrite (disk0) -  platter 0, track 07, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest07-Child4:	DiskWrite (disk0) -  platter 0, track 08, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest07-Child1:	DiskWrite (disk0) -  platter 0, track 00, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest07-Child2:	DiskWrite (disk0) -  platter 0, track 01, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest07-Child3:	DiskWrite (disk0) -  platter 0, track 02, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest07-Child4:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest07:	Test Complete
All processes completed.

DevicesTest08:	Started
DevicesTest08:	Writing to an invalid sector 
DevicesTest08:	DiskWrite sector error test PASSED
DevicesTest08:	Writing to an invalid track 
DevicesTest08:	DiskWrite track error test PASSED
DevicesTest08:	Writing to an invalid platter 
DevicesTest08:	DiskWrite track error test PASSED
All processes completed.

DevicesTest09: started
DevicesTest09-Child1:	started
DevicesTest09-Child2:	started
DevicesTest09-Child3:	started
DevicesTest09-Child4:	started
DevicesTest09-Child5:	started
DevicesTest09-Child6:	started
DevicesTest09-Child7:	started
DevicesTest09-Child8:	started
DevicesTest09-Child9:	started
DevicesTest09-Child10:	started
DevicesTest09-Child11:	started
DevicesTest09-Child12:	started
DevicesTest09-Child13:	started
DevicesTest09-Child14:	started
DevicesTest09-Child15:	started
DevicesTest09-Child16:	started
DevicesTest09-Child1:	DiskWrite (disk0) -  platter 0, track 05, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest09-Child2:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest09-Child3:	DiskWrite (disk0) -  platter 0, track 09, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest09-Child4:	DiskWrite (disk0) -  platter 0, track 00, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest09-Child5:	DiskWrite (disk0) -  platter 0, track 00, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest09-Child6:	DiskWrite (disk0) -  platter 0, track 08, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest09-Child7:	DiskWrite (disk0) -  platter 0, track 10, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest09-Child8:	DiskWrite (disk0) -  platter 0, track 15, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest09-Child9:	DiskWrite (disk0) -  platter 0, track 15, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest09-Child10:	DiskWrite (disk0) -  platter 0, track 15, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest09-Child11:	DiskWrite (disk0) -  platter 0, track 02, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest09-Child12:	DiskWrite (disk0) -  platter 0, track 01, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest09-Child13:	DiskWrite (disk0) -  platter 0, track 00, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest09-Child14:	DiskWrite (disk0) -  platter 0, track 06, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest09-Child15:	DiskWrite (disk0) -  platter 0, track 09, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest09-Child16:	DiskWrite (disk0) -  platter 0, track 09, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest09: Test complete
All processes completed.

DevicesTest10: started
DevicesTest10-Child1:	started
DevicesTest10-Child2:	started
DevicesTest10-Child3:	started
DevicesTest10-Child4:	started
DevicesTest10-Child5:	started
DevicesTest10-Child6:	started
DevicesTest10-Child7:	started
DevicesTest10-Child8:	started
DevicesTest10-Child1:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 05, sectorCount 01, status 0x00000000
DevicesTest10-Child2:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest10-Child3:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest10-Child4:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest10-Child5:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 07, sectorCount 01, status 0x00000000
DevicesTest10-Child6:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest10-Child7:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest10-Child8:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 06, sectorCount 01, status 0x00000000
DevicesTest10: Test complete
All processes completed.

DevicesTest11: started
DevicesTest11-Child1:	started
DevicesTest11-Child2:	started
DevicesTest11-Child3:	started
DevicesTest11-Child4:	started
DevicesTest11-Child5:	started
DevicesTest11-Child6:	started
DevicesTest11-Child7:	started
DevicesTest11-Child8:	started
DevicesTest11-Child9:	started
DevicesTest11-Child10:	started
DevicesTest11-Child11:	started
DevicesTest11-Child12:	started
DevicesTest11-Child13:	started
DevicesTest11-Child14:	started
DevicesTest11-Child15:	started
DevicesTest11-Child16:	started
DevicesTest11-Child1:	DiskWrite (disk0) -  platter 0, track 11, sectorStart 03, sectorCount 01, status 0x00000000
DevicesTest11-Child2:	DiskRead  (disk0) -  platter 0, track 12, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest11-Child3:	DiskWrite (disk0) -  platter 0, track 04, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest11-Child4:	DiskRead  (disk0) -  platter 0, track 05, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest11-Child5:	DiskWrite (disk0) -  platter 0, track 00, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest11-Child6:	DiskRead  (disk0) -  platter 0, track 09, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest11-Child7:	DiskWrite (disk0) -  platter 0, track 04, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest11-Child8:	DiskRead  (disk0) -  platter 0, track 12, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest11-Child9:	DiskWrite (disk0) -  platter 0, track 09, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest11-Child10:	DiskRead  (disk0) -  platter 0, track 02, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest11-Child11:	DiskWrite (disk0) -  platter 0, track 10, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest11-Child12:	DiskRead  (disk0) -  platter 0, track 00, sectorStart 00, sectorCount 01, status 0x00000000
DevicesTest11-Child13:	DiskWrite (disk0) -  platter 0, track 09, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest11-Child14:	DiskRead  (disk0) -  platter 0, track 13, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest11-Child15:	DiskWrite (disk0) -  platter 0, track 14, sectorStart 02, sectorCount 01, status 0x00000000
DevicesTest11-Child16:	DiskRead  (disk0) -  platter 0, track 13, sectorStart 01, sectorCount 01, status 0x00000000
DevicesTest11: Test complete
All processes completed.

DevicesTest12: started
DevicesTest12-Child1:	started
DevicesTest12-Child2:	started
DevicesTest12-Child2:	Sleeping for 1 seconds
DevicesTest12-Child3:	started
DevicesTest12-Child3:	Sleeping for 2 seconds
DevicesTest12-Child4:	started
DevicesTest12-Child4:	Sleeping for 3 seconds
DevicesTest12-Child5:	started
DevicesTest12-Child5:	Sleeping for 4 seconds
DevicesTest12-Child6:	started
DevicesTest12-Child7:	started
DevicesTest12-Child8:	started
DevicesTest12-Child9:	started
DevicesTest12-Child6:	DiskWrite (disk0) -  platter 0, track 07, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest12-Child8:	DiskRead  (disk1) -  platter 0, track 07, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest12-Child8:	DiskRead  (disk1) -  Output match!
DevicesTest12-Child7:	DiskWrite (disk0) -  platter 0, track 07, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest12-Child9:	DiskRead  (disk1) -  platter 0, track 07, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest12-Child9:	DiskRead  (disk1) -  Output match!
DevicesTest12-Child2:	Slept 1044 milliseconds
DevicesTest12-Child3:	Slept 2042 milliseconds
DevicesTest12-Child4:	Slept 3038 milliseconds
DevicesTest12-Child5:	Slept 4039 milliseconds
DevicesTest12: Test complete
All processes completed.

DevicesTest13: started
DevicesTest13-Child1:	started
DevicesTest13-Child1:	DiskWrite (disk0) -  platter 0, track 06, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child2:	started
DevicesTest13-Child3:	started
DevicesTest13-Child4:	started
DevicesTest13-Child5:	started
DevicesTest13-Child6:	started
DevicesTest13-Child7:	started
DevicesTest13-Child8:	started
DevicesTest13-Child2:	DiskWrite (disk0) -  platter 0, track 06, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child3:	DiskWrite (disk0) -  platter 0, track 15, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child4:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child5:	DiskWrite (disk0) -  platter 0, track 04, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child6:	DiskWrite (disk0) -  platter 0, track 07, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child7:	DiskWrite (disk0) -  platter 0, track 11, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child8:	DiskWrite (disk0) -  platter 0, track 12, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child9:	started
DevicesTest13-Child10:	started
DevicesTest13-Child11:	started
DevicesTest13-Child12:	started
DevicesTest13-Child13:	started
DevicesTest13-Child14:	started
DevicesTest13-Child15:	started
DevicesTest13-Child9:	DiskWrite (disk0) -  platter 0, track 03, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child10:	DiskWrite (disk0) -  platter 0, track 14, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child11:	DiskWrite (disk0) -  platter 0, track 04, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child12:	DiskWrite (disk0) -  platter 0, track 01, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child13:	DiskWrite (disk0) -  platter 0, track 06, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child14:	DiskWrite (disk0) -  platter 0, track 05, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13-Child15:	DiskWrite (disk0) -  platter 0, track 09, sectorStart 09, sectorCount 01, status 0x00000000
DevicesTest13: Test complete
All processes completed.


# ... Truncated Output ...
```

## Solution

Our solution implemented the FCFS approach for the seek algorithm and Doubly Linked Lists for process and I/O request management.

### Test Case Results

| Test Case Number | Test Result |
|------------------|-------------|
| 00               |    PASSED   |
| 01               |    PASSED   |
| 02               |    PASSED   |
| 03               |    PASSED   |
| 04               |    PASSED   |
| 05               |    PASSED   |
| 06               |    PASSED   |
| 07               |    PASSED   |
| 08               |    PASSED   |
| 09               |    PASSED   |
| 10               |    PASSED   |
| 11               |    PASSED   |
| 12               |    PASSED   |
| 13               |    PASSED   |

### Test Case Output

[Solution Output file](./bin/fcfsat.txt)

```bash

SystemCallsTest00: started
SystemCallsTest00: Kernel is in user mode, TEST PASSED.
All processes completed.

SystemCallsTest01: started
SystemCallsTest01: after spawn of 5
SystemCallsTest01: Parent done. Calling Exit.
launchUserProcess - Process signaled in launch.
All processes completed.

SystemCallsTest02: started and returning -1.
All processes completed.

SystemCallsTest03: started

SystemCallsTest03-Child1: started

SystemCallsTest03-Child1: Spawning one child
SystemCallsTest03-Child1-Child1: started
SystemCallsTest03-Child1-Child1: exiting
SystemCallsTest03-Child1: after spawn of child with PID 6
SystemCallsTest03-Child1: finished
SystemCallsTest03: after spawn of child with PID 5
SystemCallsTest03: Parent done. Calling Exit.
All processes completed.

SystemCallsTest04: started
SystemCallsTest04: sem_result = 0, semaphore = 0
SystemCallsTest04: sem_result = 0, semaphore = 1
All processes completed.

SystemCallsTest05: started
SystemCallsTest05: SemCreate returned  0 at index   0
SystemCallsTest05: SemCreate returned  0 at index   1
SystemCallsTest05: SemCreate returned  0 at index   2
SystemCallsTest05: SemCreate returned  0 at index   3
SystemCallsTest05: SemCreate returned  0 at index   4
SystemCallsTest05: SemCreate returned  0 at index   5
SystemCallsTest05: SemCreate returned  0 at index   6
SystemCallsTest05: SemCreate returned  0 at index   7
SystemCallsTest05: SemCreate returned  0 at index   8
SystemCallsTest05: SemCreate returned  0 at index   9
SystemCallsTest05: SemCreate returned  0 at index  10
SystemCallsTest05: SemCreate returned  0 at index  11
SystemCallsTest05: SemCreate returned  0 at index  12
SystemCallsTest05: SemCreate returned  0 at index  13
SystemCallsTest05: SemCreate returned  0 at index  14
SystemCallsTest05: SemCreate returned  0 at index  15
SystemCallsTest05: SemCreate returned  0 at index  16
SystemCallsTest05: SemCreate returned  0 at index  17
SystemCallsTest05: SemCreate returned  0 at index  18
SystemCallsTest05: SemCreate returned  0 at index  19
SystemCallsTest05: SemCreate returned  0 at index  20
SystemCallsTest05: SemCreate returned  0 at index  21
SystemCallsTest05: SemCreate returned  0 at index  22
SystemCallsTest05: SemCreate returned  0 at index  23
SystemCallsTest05: SemCreate returned  0 at index  24
SystemCallsTest05: SemCreate returned  0 at index  25
SystemCallsTest05: SemCreate returned  0 at index  26
SystemCallsTest05: SemCreate returned  0 at index  27
SystemCallsTest05: SemCreate returned  0 at index  28
SystemCallsTest05: SemCreate returned  0 at index  29
SystemCallsTest05: SemCreate returned  0 at index  30
SystemCallsTest05: SemCreate returned  0 at index  31
SystemCallsTest05: SemCreate returned  0 at index  32
SystemCallsTest05: SemCreate returned  0 at index  33
SystemCallsTest05: SemCreate returned  0 at index  34
SystemCallsTest05: SemCreate returned  0 at index  35
SystemCallsTest05: SemCreate returned  0 at index  36
SystemCallsTest05: SemCreate returned  0 at index  37
SystemCallsTest05: SemCreate returned  0 at index  38
SystemCallsTest05: SemCreate returned  0 at index  39
SystemCallsTest05: SemCreate returned  0 at index  40
SystemCallsTest05: SemCreate returned  0 at index  41
SystemCallsTest05: SemCreate returned  0 at index  42
SystemCallsTest05: SemCreate returned  0 at index  43
SystemCallsTest05: SemCreate returned  0 at index  44
SystemCallsTest05: SemCreate returned  0 at index  45
SystemCallsTest05: SemCreate returned  0 at index  46
SystemCallsTest05: SemCreate returned  0 at index  47
SystemCallsTest05: SemCreate returned  0 at index  48
SystemCallsTest05: SemCreate returned  0 at index  49
SystemCallsTest05: SemCreate returned  0 at index  50
SystemCallsTest05: SemCreate returned  0 at index  51
SystemCallsTest05: SemCreate returned  0 at index  52
SystemCallsTest05: SemCreate returned  0 at index  53
SystemCallsTest05: SemCreate returned  0 at index  54
SystemCallsTest05: SemCreate returned  0 at index  55
SystemCallsTest05: SemCreate returned  0 at index  56
SystemCallsTest05: SemCreate returned  0 at index  57
SystemCallsTest05: SemCreate returned  0 at index  58
SystemCallsTest05: SemCreate returned  0 at index  59
SystemCallsTest05: SemCreate returned  0 at index  60
SystemCallsTest05: SemCreate returned  0 at index  61
SystemCallsTest05: SemCreate returned  0 at index  62
SystemCallsTest05: SemCreate returned  0 at index  63
SystemCallsTest05: SemCreate returned  0 at index  64
SystemCallsTest05: SemCreate returned  0 at index  65
SystemCallsTest05: SemCreate returned  0 at index  66
SystemCallsTest05: SemCreate returned  0 at index  67
SystemCallsTest05: SemCreate returned  0 at index  68
SystemCallsTest05: SemCreate returned  0 at index  69
SystemCallsTest05: SemCreate returned  0 at index  70
SystemCallsTest05: SemCreate returned  0 at index  71
SystemCallsTest05: SemCreate returned  0 at index  72
SystemCallsTest05: SemCreate returned  0 at index  73
SystemCallsTest05: SemCreate returned  0 at index  74
SystemCallsTest05: SemCreate returned  0 at index  75
SystemCallsTest05: SemCreate returned  0 at index  76
SystemCallsTest05: SemCreate returned  0 at index  77
SystemCallsTest05: SemCreate returned  0 at index  78
SystemCallsTest05: SemCreate returned  0 at index  79
SystemCallsTest05: SemCreate returned  0 at index  80
SystemCallsTest05: SemCreate returned  0 at index  81
SystemCallsTest05: SemCreate returned  0 at index  82
SystemCallsTest05: SemCreate returned  0 at index  83
SystemCallsTest05: SemCreate returned  0 at index  84
SystemCallsTest05: SemCreate returned  0 at index  85
SystemCallsTest05: SemCreate returned  0 at index  86
SystemCallsTest05: SemCreate returned  0 at index  87
SystemCallsTest05: SemCreate returned  0 at index  88
SystemCallsTest05: SemCreate returned  0 at index  89
SystemCallsTest05: SemCreate returned  0 at index  90
SystemCallsTest05: SemCreate returned  0 at index  91
SystemCallsTest05: SemCreate returned  0 at index  92
SystemCallsTest05: SemCreate returned  0 at index  93
SystemCallsTest05: SemCreate returned  0 at index  94
SystemCallsTest05: SemCreate returned  0 at index  95
SystemCallsTest05: SemCreate returned  0 at index  96
SystemCallsTest05: SemCreate returned  0 at index  97
SystemCallsTest05: SemCreate returned  0 at index  98
SystemCallsTest05: SemCreate returned  0 at index  99
SystemCallsTest05: SemCreate returned  0 at index 100
SystemCallsTest05: SemCreate returned  0 at index 101
SystemCallsTest05: SemCreate returned  0 at index 102
SystemCallsTest05: SemCreate returned  0 at index 103
SystemCallsTest05: SemCreate returned  0 at index 104
SystemCallsTest05: SemCreate returned  0 at index 105
SystemCallsTest05: SemCreate returned  0 at index 106
SystemCallsTest05: SemCreate returned  0 at index 107
SystemCallsTest05: SemCreate returned  0 at index 108
SystemCallsTest05: SemCreate returned  0 at index 109
SystemCallsTest05: SemCreate returned  0 at index 110
SystemCallsTest05: SemCreate returned  0 at index 111
SystemCallsTest05: SemCreate returned  0 at index 112
SystemCallsTest05: SemCreate returned  0 at index 113
SystemCallsTest05: SemCreate returned  0 at index 114
SystemCallsTest05: SemCreate returned  0 at index 115
SystemCallsTest05: SemCreate returned  0 at index 116
SystemCallsTest05: SemCreate returned  0 at index 117
SystemCallsTest05: SemCreate returned  0 at index 118
SystemCallsTest05: SemCreate returned  0 at index 119
SystemCallsTest05: SemCreate returned  0 at index 120
SystemCallsTest05: SemCreate returned  0 at index 121
SystemCallsTest05: SemCreate returned  0 at index 122
SystemCallsTest05: SemCreate returned  0 at index 123
SystemCallsTest05: SemCreate returned  0 at index 124
SystemCallsTest05: SemCreate returned  0 at index 125
SystemCallsTest05: SemCreate returned  0 at index 126
SystemCallsTest05: SemCreate returned  0 at index 127
SystemCallsTest05: SemCreate returned  0 at index 128
SystemCallsTest05: SemCreate returned  0 at index 129
SystemCallsTest05: SemCreate returned  0 at index 130
SystemCallsTest05: SemCreate returned  0 at index 131
SystemCallsTest05: SemCreate returned  0 at index 132
SystemCallsTest05: SemCreate returned  0 at index 133
SystemCallsTest05: SemCreate returned  0 at index 134
SystemCallsTest05: SemCreate returned  0 at index 135
SystemCallsTest05: SemCreate returned  0 at index 136
SystemCallsTest05: SemCreate returned  0 at index 137
SystemCallsTest05: SemCreate returned  0 at index 138
SystemCallsTest05: SemCreate returned  0 at index 139
SystemCallsTest05: SemCreate returned  0 at index 140
SystemCallsTest05: SemCreate returned  0 at index 141
SystemCallsTest05: SemCreate returned  0 at index 142
SystemCallsTest05: SemCreate returned  0 at index 143
SystemCallsTest05: SemCreate returned  0 at index 144
SystemCallsTest05: SemCreate returned  0 at index 145
SystemCallsTest05: SemCreate returned  0 at index 146
SystemCallsTest05: SemCreate returned  0 at index 147
SystemCallsTest05: SemCreate returned  0 at index 148
SystemCallsTest05: SemCreate returned  0 at index 149
SystemCallsTest05: SemCreate returned  0 at index 150
SystemCallsTest05: SemCreate returned  0 at index 151
SystemCallsTest05: SemCreate returned  0 at index 152
SystemCallsTest05: SemCreate returned  0 at index 153
SystemCallsTest05: SemCreate returned  0 at index 154
SystemCallsTest05: SemCreate returned  0 at index 155
SystemCallsTest05: SemCreate returned  0 at index 156
SystemCallsTest05: SemCreate returned  0 at index 157
SystemCallsTest05: SemCreate returned  0 at index 158
SystemCallsTest05: SemCreate returned  0 at index 159
SystemCallsTest05: SemCreate returned  0 at index 160
SystemCallsTest05: SemCreate returned  0 at index 161
SystemCallsTest05: SemCreate returned  0 at index 162
SystemCallsTest05: SemCreate returned  0 at index 163
SystemCallsTest05: SemCreate returned  0 at index 164
SystemCallsTest05: SemCreate returned  0 at index 165
SystemCallsTest05: SemCreate returned  0 at index 166
SystemCallsTest05: SemCreate returned  0 at index 167
SystemCallsTest05: SemCreate returned  0 at index 168
SystemCallsTest05: SemCreate returned  0 at index 169
SystemCallsTest05: SemCreate returned  0 at index 170
SystemCallsTest05: SemCreate returned  0 at index 171
SystemCallsTest05: SemCreate returned  0 at index 172
SystemCallsTest05: SemCreate returned  0 at index 173
SystemCallsTest05: SemCreate returned  0 at index 174
SystemCallsTest05: SemCreate returned  0 at index 175
SystemCallsTest05: SemCreate returned  0 at index 176
SystemCallsTest05: SemCreate returned  0 at index 177
SystemCallsTest05: SemCreate returned  0 at index 178
SystemCallsTest05: SemCreate returned  0 at index 179
SystemCallsTest05: SemCreate returned  0 at index 180
SystemCallsTest05: SemCreate returned  0 at index 181
SystemCallsTest05: SemCreate returned  0 at index 182
SystemCallsTest05: SemCreate returned  0 at index 183
SystemCallsTest05: SemCreate returned  0 at index 184
SystemCallsTest05: SemCreate returned  0 at index 185
SystemCallsTest05: SemCreate returned  0 at index 186
SystemCallsTest05: SemCreate returned  0 at index 187
SystemCallsTest05: SemCreate returned  0 at index 188
SystemCallsTest05: SemCreate returned  0 at index 189
SystemCallsTest05: SemCreate returned  0 at index 190
SystemCallsTest05: SemCreate returned  0 at index 191
SystemCallsTest05: SemCreate returned  0 at index 192
SystemCallsTest05: SemCreate returned  0 at index 193
SystemCallsTest05: SemCreate returned  0 at index 194
SystemCallsTest05: SemCreate returned  0 at index 195
SystemCallsTest05: SemCreate returned  0 at index 196
SystemCallsTest05: SemCreate returned  0 at index 197
SystemCallsTest05: SemCreate returned  0 at index 198
SystemCallsTest05: SemCreate returned  0 at index 199
SystemCallsTest05: SemCreate returned -1 at index 200
SystemCallsTest05: SemCreate returned -1 at index 201
All processes completed.

SystemCallsTest06: started
SystemCallsTest06: sem_result = 0, semaphore = 0
SystemCallsTest06-Child1: started
SystemCallsTest06-Child1: Calling SemP on semaphore 0
SystemCallsTest06: after spawn of child with PID 5
SystemCallsTest06-Child2: started
SystemCallsTest06-Child2: Calling SemV on semaphore 0
SystemCallsTest06-Child2: SemV returned 0
SystemCallsTest06-Child2: exiting
SystemCallsTest06-Child1: SemP returned 0
SystemCallsTest06-Child1: exiting
SystemCallsTest06: after spawn of child with PID 6
SystemCallsTest06: Wait returned for child with PID 6 and status 9
SystemCallsTest06: Wait returned for child with PID 5 and status 9
SystemCallsTest06: Parent done. Calling Exit.
All processes completed.

SystemCallsTest07: started
SystemCallsTest07: sem_result = 0, semaphore = 0
SystemCallsTest07-Child1: started
SystemCallsTest07-Child1: Calling SemP on semaphore 0
SystemCallsTest07: after spawn of child with PID 5
SystemCallsTest07-Child2: started
SystemCallsTest07-Child2: Calling SemP on semaphore 0
SystemCallsTest07: after spawn of child with PID 6
SystemCallsTest07-Child3: started
SystemCallsTest07-Child3: Calling SemP on semaphore 0
SystemCallsTest07: after spawn of child with PID 7
SystemCallsTest07-Child4: started
SystemCallsTest07-Child4: Calling SemV on semaphore 0
SystemCallsTest07-Child4: SemV returned 0
SystemCallsTest07-Child4: Calling SemV on semaphore 0
SystemCallsTest07-Child4: SemV returned 0
SystemCallsTest07-Child4: Calling SemV on semaphore 0
SystemCallsTest07-Child4: SemV returned 0
SystemCallsTest07-Child4: exiting
SystemCallsTest07-Child1: SemP returned 0
SystemCallsTest07-Child1: exiting
SystemCallsTest07-Child2: SemP returned 0
SystemCallsTest07-Child2: exiting
SystemCallsTest07-Child3: SemP returned 0
SystemCallsTest07-Child3: exiting
SystemCallsTest07: after spawn of child with PID 8
SystemCallsTest07: Parent done. Calling Exit.
All processes completed.

SystemCallsTest08: started
SystemCallsTest08: Creating MAXSEMS semaphores
SystemCallsTest08: Freeing one semaphore
SystemCallsTest08: TEST PASSED - SemCreate returned  0
All processes completed.

SystemCallsTest09: started
SystemCallsTest09: sem_result = 0, semaphore = 0
SystemCallsTest09-Child1: started
SystemCallsTest09-Child1: Calling SemP on semaphore 0
SystemCallsTest09: after spawn of child with PID 5
SystemCallsTest09-Child2: started
SystemCallsTest09-Child2: Calling SemP on semaphore 0
SystemCallsTest09: after spawn of child with PID 6
SystemCallsTest09-Child3: started
SystemCallsTest09-Child3: Calling SemP on semaphore 0
SystemCallsTest09: after spawn of child with PID 7
SystemCallsTest09-Child4: started
SystemCallsTest09-Child4: Calling SemFree on semaphore 0
SystemCallsTest09-Child4: SemFree returned 1 on semaphore 0
SystemCallsTest09-Child4: exiting
SystemCallsTest09: after spawn of child with PID 8
SystemCallsTest09: Parent done. Calling Exit.
All processes completed.

SystemCallsTest10: started
SystemCallsTest10-Child1: started
SystemCallsTest10-Child1: Sleep complete - 2000768
SystemCallsTest10: after spawn of child with PID 5
SystemCallsTest10-Child1: started
SystemCallsTest10-Child1: Sleep complete - 2005838
SystemCallsTest10: after spawn of child with PID 6
SystemCallsTest10-Child1: started
SystemCallsTest10-Child1: Sleep complete - 2006096
SystemCallsTest10: after spawn of child with PID 7
SystemCallsTest10: Wait returned for child with PID 5 and status 9
SystemCallsTest10: Wait returned for child with PID 6 and status 9
SystemCallsTest10: Wait returned for child with PID 7 and status 9
SystemCallsTest10: Parent done. Calling Exit.
All processes completed.

SystemCallsTest11: started
SystemCallsTest11-Child1: started
SystemCallsTest11-Child1: Sleep complete - 2005
SystemCallsTest11: after spawn of child with PID 5
SystemCallsTest11-Child1: started
SystemCallsTest11-Child1: Sleep complete - 2006
SystemCallsTest11: after spawn of child with PID 6
SystemCallsTest11-Child1: started
SystemCallsTest11-Child1: Sleep complete - 2003
SystemCallsTest11: after spawn of child with PID 7
SystemCallsTest11: Wait returned for child with PID 5 and status 9
SystemCallsTest11: Wait returned for child with PID 6 and status 9
SystemCallsTest11: Wait returned for child with PID 7 and status 9
SystemCallsTest11: Parent done. Calling Exit.
All processes completed.

SystemCallsTest12: started
SystemCallsTest12-Child1: started
SystemCallsTest12-Child1: my PID is 5
SystemCallsTest12: after spawn of child with PID 5
SystemCallsTest12-Child1: started
SystemCallsTest12-Child1: my PID is 6
SystemCallsTest12: after spawn of child with PID 6
SystemCallsTest12-Child1: started
SystemCallsTest12-Child1: my PID is 7
SystemCallsTest12: after spawn of child with PID 7
SystemCallsTest12: Wait returned for child with PID 5 and status 9
SystemCallsTest12: Wait returned for child with PID 6 and status 9
SystemCallsTest12: Wait returned for child with PID 7 and status 9
SystemCallsTest12: Parent done. Calling Exit.
All processes completed.

SystemCallsTest13: started
SystemCallsTest13: sem_result = 0, semaphore = 0
SystemCallsTest13-Child1: started
SystemCallsTest13-Child1: Calling SemP on semaphore 0
SystemCallsTest13: after spawn of child with PID 5
SystemCallsTest13-Child2: started
SystemCallsTest13-Child2: Calling SemP on semaphore 0
SystemCallsTest13: after spawn of child with PID 6
SystemCallsTest13: After SemFree with processes blocked
SystemCallsTest13: Parent done. Calling Exit.
All processes completed.

SystemCallsTest14: started
SystemCallsTest14: after spawn of 5
SystemCallsTest14-Child1: started
SystemCallsTest14-Child1: exiting
SystemCallsTest14: Wait returned for child with PID 5 and status 9
SystemCallsTest14: Parent done. Calling Exit.
All processes completed.

SystemCallsTest15: started
SystemCallsTest15: after spawn of child with PID 5
SystemCallsTest15-Child1: started
SystemCallsTest15-Child1-Child1: started
SystemCallsTest15-Child1-Child1: after spawn of 7
SystemCallsTest15-Child1-Child1: after spawn of 8
SystemCallsTest15-Child1-Child1: after spawn of 9
SystemCallsTest15-Child1: after spawn of 6
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
SystemCallsTest15-Child1: Wait returned for child with PID 6 and status 10
SystemCallsTest15-Child1: after spawn of 10
SystemCallsTest15-Child1-Child2: started
SystemCallsTest15-Child1-Child2: exiting
SystemCallsTest15-Child1: Wait returned for child with PID 10 and status 9
SystemCallsTest15-Child1: Parent done. Calling Exit.
SystemCallsTest15: Wait returned for child with PID 5 and status 9
SystemCallsTest15: Parent done. Calling Exit.
All processes completed.

SystemCallsTest16: started
SystemCallsTest16: sem_result = 0, semaphore = 0
SystemCallsTest16-Child1: started
SystemCallsTest16-Child1: Calling SemP on semaphore 0
SystemCallsTest16-Child1: SemP returned 0
SystemCallsTest16-Child1: Calling SemP on semaphore 0
SystemCallsTest16-Child1: SemP returned 0
SystemCallsTest16-Child1: Calling SemP on semaphore 0
SystemCallsTest16: after spawn of child with PID 5
SystemCallsTest16: after spawn of child with PID 6
SystemCallsTest16-Child1: SemP returned 0
SystemCallsTest16-Child1: Calling SemP on semaphore 0
SystemCallsTest16-Child2: started
SystemCallsTest16-Child2: Calling SemV on semaphore 0
SystemCallsTest16-Child1: SemP returned 0
SystemCallsTest16-Child1: Calling SemP on semaphore 0
SystemCallsTest16: after Semv
SystemCallsTest16-Child2: SemV returned 0
SystemCallsTest16-Child2: Calling SemV on semaphore 0
SystemCallsTest16-Child1: SemP returned 0
SystemCallsTest16-Child1: exiting
SystemCallsTest16-Child2: SemV returned 0
SystemCallsTest16-Child2: exiting
SystemCallsTest16: Wait returned for child with PID 5 and status 9
SystemCallsTest16: Wait returned for child with PID 6 and status 9
SystemCallsTest16: Parent done. Calling Exit.
All processes completed.

SystemCallsTest17: started
SystemCallsTest17: after spawn of child with PID 5
SystemCallsTest17-Child1: started
SystemCallsTest17-Child1-Child1: started
SystemCallsTest17-Child1-Child1: after spawn of 7
SystemCallsTest17-Child1-Child1: after spawn of 8
SystemCallsTest17-Child1-Child1: after spawn of 9
SystemCallsTest17-Child1-Child1: after spawn of 10
SystemCallsTest17-Child1-Child1: after spawn of 11
SystemCallsTest17-Child1-Child1: after spawn of 12
SystemCallsTest17-Child1-Child1: after spawn of 13
SystemCallsTest17-Child1-Child1: after spawn of 14
SystemCallsTest17-Child1-Child1: after spawn of 15
SystemCallsTest17-Child1-Child1: after spawn of 16
SystemCallsTest17-Child1-Child1: after spawn of 17
SystemCallsTest17-Child1-Child1: after spawn of 18
SystemCallsTest17-Child1-Child1: after spawn of 19
SystemCallsTest17-Child1-Child1: after spawn of 20
SystemCallsTest17-Child1-Child1: after spawn of 21
SystemCallsTest17-Child1-Child1: after spawn of 22
SystemCallsTest17-Child1-Child1: after spawn of 23
SystemCallsTest17-Child1-Child1: after spawn of 24
SystemCallsTest17-Child1-Child1: after spawn of 25
SystemCallsTest17-Child1-Child1: after spawn of 26
SystemCallsTest17-Child1-Child1: after spawn of 27
SystemCallsTest17-Child1-Child1: after spawn of 28
SystemCallsTest17-Child1-Child1: after spawn of 29
SystemCallsTest17-Child1-Child1: after spawn of 30
SystemCallsTest17-Child1-Child1: after spawn of 31
SystemCallsTest17-Child1-Child1: after spawn of 32
SystemCallsTest17-Child1-Child1: after spawn of 33
SystemCallsTest17-Child1-Child1: after spawn of 34
SystemCallsTest17-Child1-Child1: after spawn of 35
SystemCallsTest17-Child1-Child1: after spawn of 36
SystemCallsTest17-Child1-Child1: after spawn of 37
SystemCallsTest17-Child1-Child1: after spawn of 38
SystemCallsTest17-Child1-Child1: after spawn of 39
SystemCallsTest17-Child1-Child1: after spawn of 40
SystemCallsTest17-Child1-Child1: after spawn of 41
SystemCallsTest17-Child1-Child1: after spawn of 42
SystemCallsTest17-Child1-Child1: after spawn of 43
SystemCallsTest17-Child1-Child1: after spawn of 44
SystemCallsTest17-Child1-Child1: after spawn of 45
SystemCallsTest17-Child1-Child1: after spawn of 46
SystemCallsTest17-Child1-Child1: after spawn of 47
SystemCallsTest17-Child1-Child1: after spawn of 48
SystemCallsTest17-Child1-Child1: after spawn of 49
SystemCallsTest17-Child1-Child1: after spawn of 50
SystemCallsTest17-Child1-Child1: after spawn of -4
SystemCallsTest17-Child1-Child1: after spawn of -4
SystemCallsTest17-Child1-Child1: after spawn of -4
SystemCallsTest17-Child1-Child1: after spawn of -4
SystemCallsTest17-Child1-Child1: after spawn of -4
SystemCallsTest17-Child1-Child1: after spawn of -4
SystemCallsTest17-Child1-Child1: Exiting and terminating all child processes.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
launchUserProcess - Process signaled in launch.
SystemCallsTest17-Child1: after spawn of 6
SystemCallsTest17-Child1: Wait returned for child with PID 6 and status 9
SystemCallsTest17-Child1-Child2: started with PID 56
SystemCallsTest17-Child1: after spawn of 56
SystemCallsTest17-Child1: Wait returned for child with PID 56 and status 0
SystemCallsTest17-Child1: Parent done. Calling Exit.
SystemCallsTest17: Wait returned for child with PID 5 and status 9
SystemCallsTest17: Parent done. Calling Exit.
All processes completed.

```
