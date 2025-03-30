# System Calls Project Solution

## CYBV 489 Principles of Operating Systems

`Anthony Tropeano` `Dan Tiger`

## Table of Contents

- [Overview](#system-calls-project-scope)
- [Contributors](#contributors)
- [Usage](#usage)
- [Solution](#solution)

## System Calls Project Scope

The third stage of our OS development project introduces user level processes that access kernel
features through system calls. We also implement functionality for semaphores based on the
mailbox system developed in the Messaging project.

### User Level Processes and System Calls

- The System Calls project enables processes to run in user mode while accessing the kernel-level features developed in the Scheduler and Messaging projects
- Processes created, or spawned, in this phase processes cannot directly access kernel-levelfunctions like k_spawn or k_wait. Instead, the processes access these functions through asystem call which traps the kernel and executes the call on the process’s behalf. The kernel is trapped through an interrupt that occurs when a system call is made. This interrupt,THREADS_SYS_CALL_INTERRUPT, invokes a system call handler that is responsible for the system call’s implementation. 
- Some good news is that the THREADS_SYS_CALL_INTERRUPT handler function is already implemented and is part of the provided Messaging project. The Messaging project sets the system call interrupt handler upon kernel start up and will invoke system calls through a system call vector table called systemCallVector. This vector is defined for you in the starter project. The vector holds a function pointer for each of the system calls that the kernel supports.
- For the System Calls project, we are implementing the system calls defined in Table

| System Call Id | User Process Interface |
| --- | --- |
| SYS_SPAWN | Spawn |
| SYS_WAIT | Wait |
|SYS_EXIT | Exit |
|SYS_SLEEP | SleepSeconds |
|SYS_SEMCREATE | SemCreate |
|SYS_SEMP | SemP |
|SYS_SEMV | SemV |
|SYS_SEMFREE | SemFree |
|SYS_GETTIMEOFDAY | GetTimeofDay |
|SYS_CPUTIME | CPUTime |
|SYS_GETPID | GetPID |

- The System Calls project implementation must set a handler for each of these system calls. The following code illustrates how to set a handler for the Spawn call as an example.
```C
 systemCallVector[SYS_SPAWN] = spawn_system_call_handler; 
 ```

### Semaphores

- The System Call project exposes several kernel level functions that were created during the Scheduler project. These include spawn, wait, exit, and the timing functions. We are also adding kernel functions for semaphores with the addition of the following functions:

> 1. k_semcreate – Create a semaphore
> 1. k_semp – Decrement the semaphore
> 1. k_semv – Increment the semaphore
> 1. k_semfree – Free the semaphore

- Semaphore management is performed using kernel-level functions. Semaphores each have a unique identifier that is returned from the k_semcreate function. 
- Semaphores are incremented and decremented using the k_semv and k_semp functions, respectively. The k_semfree
releases a semaphore. 

## Contributors

| Name | College | Program | Contact |
| --- | --- | --- | --- |
| Anthony Tropeano | [**College of Applied Science and Technology**](https://azcast.arizona.edu/) | [*Cyber Operations/Cyber Engineering*](https://azcast.arizona.edu/academics/cyber-operations/cyber-engineering) | [**GitHub**](https://github.com/iiTONELOC) <br> [**Email**](mailto:atropeano@arizona.edu) |
| Dan Tiger | [**College of Applied Science and Technology**](https://azcast.arizona.edu/) | [*Applied Computing: Software Development*](https://azcast.arizona.edu/academics/applied-computing/software-development) | [**GitHub**](https://github.com/dtigergh) <br> [**Email**](mailto:dtiger@arizona.edu) |

## Usage

Part of the solution utilizes a Generic Linked List implementation, [DoubleSeaLib](https://github.com/iiTONELOC/DoubleSea) created by Anthony, as a DLL, and is excluded from the `threads-kernel` repo.

For simplicity, a ZIP folder has been provided which includes pre-built output and the necessary DLLs and Libs.

The solution folder can be opened in Visual Studio and the project rebuilt if necessary.

Once unzipped, the prebuilt solution and its tests can be executed directly using the Batch file included in the `bin/` folder titled: `RunSysCallTests.bat`:

```bash
cd <path_to_THREADS-SystemCalls/bin>

.\RunSysCallTests.bat

# expected output

SystemCallsTest00: started
All processes completed.

# ... Truncated Output ...
```

## Solution

### Test Case Results

| Test Case Number | Test Result |
|------------------|-------------|
| 00               |    PASSED   |


>Note: Test cases with an * indicated that the test passes but does not match the provided output when piped to a file.

### Test Case Output

[Solution Output file](bin\\ExpectedSysCallOut.txt)
```bash

SystemCallsTest00: started
SystemCallsTest00: Kernel is in user mode, TEST PASSED.
All processes completed.

SystemCallsTest01: started
SystemCallsTest01: after spawn of 5
SystemCallsTest01: Parent done. Calling Exit.
SystemCallsTest01-Child1: started
SystemCallsTest01-Child1: exiting
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
SystemCallsTest10-Child1: Sleep complete - 2003407
SystemCallsTest10: after spawn of child with PID 5
SystemCallsTest10-Child1: started
SystemCallsTest10-Child1: Sleep complete - 2002125
SystemCallsTest10: after spawn of child with PID 6
SystemCallsTest10-Child1: started
SystemCallsTest10-Child1: Sleep complete - 2001599
SystemCallsTest10: after spawn of child with PID 7
SystemCallsTest10: Wait returned for child with PID 5 and status 9
SystemCallsTest10: Wait returned for child with PID 6 and status 9
SystemCallsTest10: Wait returned for child with PID 7 and status 9
SystemCallsTest10: Parent done. Calling Exit.
All processes completed.

SystemCallsTest11: started
SystemCallsTest11-Child1: started
SystemCallsTest11-Child1: Sleep complete - 2002
SystemCallsTest11: after spawn of child with PID 5
SystemCallsTest11-Child1: started
SystemCallsTest11-Child1: Sleep complete - 2008
SystemCallsTest11: after spawn of child with PID 6
SystemCallsTest11-Child1: started
SystemCallsTest11-Child1: Sleep complete - 2002
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
SystemCallsTest15-Child1-Child1-Child1: started
SystemCallsTest15-Child1-Child1-Child1: exiting
SystemCallsTest15-Child1-Child1-Child2: started
SystemCallsTest15-Child1-Child1-Child2: exiting
SystemCallsTest15-Child1-Child1-Child3: started
SystemCallsTest15-Child1-Child1-Child3: exiting
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
Failed to create user process.SystemCallsTest17-Child1-Child1: after spawn of -4
Failed to create user process.SystemCallsTest17-Child1-Child1: after spawn of -4
Failed to create user process.SystemCallsTest17-Child1-Child1: after spawn of -4
Failed to create user process.SystemCallsTest17-Child1-Child1: after spawn of -4
Failed to create user process.SystemCallsTest17-Child1-Child1: after spawn of -4
Failed to create user process.SystemCallsTest17-Child1-Child1: after spawn of -4
SystemCallsTest17-Child1-Child1: Exiting and terminating all child processes.
SystemCallsTest17-Child1-Child1-Child1: started with PID 7
SystemCallsTest17-Child1-Child1-Child2: started with PID 8
SystemCallsTest17-Child1-Child1-Child3: started with PID 9
SystemCallsTest17-Child1-Child1-Child4: started with PID 10
SystemCallsTest17-Child1-Child1-Child5: started with PID 11
SystemCallsTest17-Child1-Child1-Child6: started with PID 12
SystemCallsTest17-Child1-Child1-Child7: started with PID 13
SystemCallsTest17-Child1-Child1-Child8: started with PID 14
SystemCallsTest17-Child1-Child1-Child9: started with PID 15
SystemCallsTest17-Child1-Child1-Child10: started with PID 16
SystemCallsTest17-Child1-Child1-Child11: started with PID 17
SystemCallsTest17-Child1-Child1-Child12: started with PID 18
SystemCallsTest17-Child1-Child1-Child13: started with PID 19
SystemCallsTest17-Child1-Child1-Child14: started with PID 20
SystemCallsTest17-Child1-Child1-Child15: started with PID 21
SystemCallsTest17-Child1-Child1-Child16: started with PID 22
SystemCallsTest17-Child1-Child1-Child17: started with PID 23
SystemCallsTest17-Child1-Child1-Child18: started with PID 24
SystemCallsTest17-Child1-Child1-Child19: started with PID 25
SystemCallsTest17-Child1-Child1-Child20: started with PID 26
SystemCallsTest17-Child1-Child1-Child21: started with PID 27
SystemCallsTest17-Child1-Child1-Child22: started with PID 28
SystemCallsTest17-Child1-Child1-Child23: started with PID 29
SystemCallsTest17-Child1-Child1-Child24: started with PID 30
SystemCallsTest17-Child1-Child1-Child25: started with PID 31
SystemCallsTest17-Child1-Child1-Child26: started with PID 32
SystemCallsTest17-Child1-Child1-Child27: started with PID 33
SystemCallsTest17-Child1-Child1-Child28: started with PID 34
SystemCallsTest17-Child1-Child1-Child29: started with PID 35
SystemCallsTest17-Child1-Child1-Child30: started with PID 36
SystemCallsTest17-Child1-Child1-Child31: started with PID 37
SystemCallsTest17-Child1-Child1-Child32: started with PID 38
SystemCallsTest17-Child1-Child1-Child33: started with PID 39
SystemCallsTest17-Child1-Child1-Child34: started with PID 40
SystemCallsTest17-Child1-Child1-Child35: started with PID 41
SystemCallsTest17-Child1-Child1-Child36: started with PID 42
SystemCallsTest17-Child1-Child1-Child37: started with PID 43
SystemCallsTest17-Child1-Child1-Child38: started with PID 44
SystemCallsTest17-Child1-Child1-Child39: started with PID 45
SystemCallsTest17-Child1-Child1-Child40: started with PID 46
SystemCallsTest17-Child1-Child1-Child41: started with PID 47
SystemCallsTest17-Child1-Child1-Child42: started with PID 48
SystemCallsTest17-Child1-Child1-Child43: started with PID 49
SystemCallsTest17-Child1-Child1-Child44: started with PID 50
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
