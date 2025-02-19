# Scheduler Project Solution

## CYBV 489 Principles of Operating Systems

## Table of Contents

- [Overview](#overview)
- [Contributors](#contributors)
- [Usage](#usage)
- [Solution](#solution)

## Overview

The Scheduler project builds the functionality in the kernel that controls processes at the lowest level. This the part of the kernel that creates processes, manages which process runs and for how long, manages process state and interprocess relationships, and much more.

The primary responsibility of the Scheduler is process management and the initialization of the kernel’s core elements. The Scheduler encapsulates the central features needed throughout kernel’s lifecycle from boot to process management to system shutdown.  

This includes:
 * Starting or booting the kernel
 * Scheduling processes to run
 * Managing process states (starting, running, and terminating)
 * Timer interrupt handling
 * System shutdown
  
The Scheduler is the first step in an iterative development of the whole kernel where each iteration of the development builds upon the previous

```md
  +-------------------------------------+
  |             Input/Output            |
  |                                     |
  +-------------------------------------+
  |            User Processing          |
  |                   +-----------------|
  |                   |    Messaging    |
  +-------------------------------------+
  |             Scheduler               |
  |                                     |
  +-------------------------------------+
 ```

>Figure 1 The Scheduler is the foundation of the kernel project.
>

## Contributors

| Name | College | Program | Contact |
| --- | --- | --- | --- |
| Anthony Tropeano | [**College of Applied Science and Technology**](https://azcast.arizona.edu/) | [*Cyber Operations/Cyber Engineering*](https://azcast.arizona.edu/academics/cyber-operations/cyber-engineering) | [**GitHub**](https://github.com/iiTONELOC) <br> [**Email**](mailto:atropeano@arizona.edu) |
| Connor Stackhouse | [**College of Applied Science and Technology**](https://azcast.arizona.edu/) | [*Cyber Operations/Cyber Engineering*](https://azcast.arizona.edu/academics/cyber-operations/cyber-engineering) | [**GitHub**](https://github.com/con-stackhouse) <br> [**Email**](mailto:cstackhouse@arizona.edu) |

## Usage

All modified files have been provided with the submission and will need to be placed at the solution's root for a successful build, including the `Scheduler.vcxproj` file. Additionally, a zipped THREADS-Scheduler, `SchedulerSolution\THREADS-Scheduler-SolutionCAST.zip` has also been provided. It is pre-built and contains everything needed to be instantly executed from within `Visual Studio`.  

## Solution

The current approach for the Scheduler uses doubly linked lists and a priority-process queue for managing processes.  

The queue isn't really a queue, but it is an array of linked lists, indexed by state, i.e. Ready, Running, blocked, etc., with each list sorted by priority.

The process table, storage for linked list nodes, and the priority queue are all pre-allocated at compile-time to the stack.

Our solution, while it may be changed, currently implements linked lists as virtual lists, therefore a linked list node should only exist in one list at a time. Linked List Nodes for children processes are dynamically allocated when the child is created and destroyed when the child exits.

```c
Process processTable[MAX_PROCESSES];            
```

### Priority-Round-Robin Scheduling Model

```md
+-----------+
| 0 READY   | --> [ Priority 4 ] --> [ Priority 4 ] --> [ Priority 3 ] --> [ Priority 1 ]
+-----------+

+-----------+
| 1 RUNNING | --> [ Priority 4 ]
+-----------+

+-----------+
| 2 BLOCKED | --> [ Priority 4 ] --> [ Priority 3 ] --> [ Priority 3 ]
+-----------+

+-----------+
| 3 QUIT    | --> [ Priority 3 ]
+-----------+
```

### Process Control Block (PCB)

```c
typedef struct _process
{
	struct _process *pParent;
	DoublyLinkedList pChildren;
	DoublyLinkedList pDeadChildren;
	DoublyLinkedList pExitingChildren;
	DoublyLinkedList pJoiningProcesses;

	short pid;					/* Process id (pid) */
	int status;					/* READY, QUIT, BLOCKED, etc. */
	int signal;					/* Signal to send to process */
	char *stack;				/* Process stack */
	int exitCode;				/* Process exit code */
	int priority;				/* Process priority */
	void *context;				/* Process's current context */
	int joinStatus;				/*Status from a process this process is trying to join*/
	char name[MAXNAME];			/* Process name */
	unsigned int quantum;		/* Time slice */
	int processTableIndex;		/* Index into the process table */
	unsigned int stacksize;		/* Process stack size */
	char startArgs[MAXARG];		/* Process arguments */
	unsigned int startTime;		/* Process start time */
	unsigned int elapsedTime;	/* Process elapsed time */
	int (*entryPoint)(void *);	/* The entry point that is called from launch */
	unsigned long long cpuTime; /* Process CPU time */

} Process;
```

### Doubly Linked Lists

```c
typedef struct DoublyLinkedNode
{
    void* pData;
    unsigned short dynamic;
    struct DoublyLinkedNode* pNext;
    struct DoublyLinkedNode* pPrev;
} DoublyLinkedNode;


typedef struct DoublyLinkedList
{
    int count;                                        // Number of nodes in the list
    unsigned short dynamic;                           // Dynamic allocation ?
    DoublyLinkedNode *pHead;                          // Pointer to the head of the list
    DoublyLinkedNode *pTail;                          // Pointer to the tail of the list
    int (*OrderFunction)(void *pNode1, void *pNode2); // Pointer to an OrderFunction
} DoublyLinkedList;
```


### Test Case Results

| Test Case Number | Test Result                            |
|------------------|----------------------------------------|
| 00               | <p style="color: green"> PASSED </p>   |
| 01               | <p style="color: green"> PASSED </p>   |
| 02               | <p style="color: green"> PASSED </p>   |
| 03               | <p style="color: green"> PASSED </p>   |
| 04               | <p style="color: green"> PASSED </p>   |
| 05               | <p style="color: green"> PASSED </p>   |
| 06               | <p style="color: green"> PASSED </p>   |
| 07               | <p style="color: green"> PASSED </p>   |
| 08               | <p style="color: green"> PASSED </p>   |
| 09               | <p style="color: green"> PASSED </p>   |
| 10               | <p style="color: green"> PASSED </p>   |
| 11               | <p style="color: green"> PASSED </p>   |
| 12               | <p style="color: green"> PASSED </p>   |
| 13               | <p style="color: green"> PASSED </p>   |
| 14               | <p style="color: green"> PASSED </p>   |
| 15               | <p style="color: green"> PASSED </p>   |
| 16               | <p style="color: green"> PASSED </p>   |
| 17               | <p style="color: green"> PASSED </p>   |
| 18               | <p style="color: green"> PASSED </p>   |
| 19               | <p style="color: green"> PASSED </p>   |
| 20               | <p style="color: green"> PASSED </p>   |
| 21               | <p style="color: green"> PASSED </p>   |
| 22               | <p style="color: green"> PASSED </p>   |
| 23               | <p style="color: green"> PASSED </p>   |
| 24               | <p style="color: green"> PASSED </p>   |
| 25               | <p style="color: green"> PASSED </p>   |
| 26               | <p style="color: green"> PASSED </p>   |
| 27               | <p style="color: green"> PASSED </p>   |
| 28               | <p style="color: green"> PASSED </p>   |
| 29               | <p style="color: green"> PASSED </p>   |
| 30               | <p style="color: green"> PASSED </p>   |
| 31               | <p style="color: green"> PASSED </p>   |


### Test Case Output

```bash


SchedulerTest00: started
SchedulerTest00: after spawn of child with pid 3
SchedulerTest00: waiting for child process
SchedulerTest00-Child1: started
SchedulerTest00-Child1: quitting
SchedulerTest00: exit status for child 3 is -3
All processes completed.

SchedulerTest01: started
SchedulerTest01: after spawn of child with pid 3
SchedulerTest01: waiting for child process
SchedulerTest01-Child1: started
SchedulerTest01-Child1: performing spawn of first child
SchedulerTest01-Child1: spawn of first child returned pid = 4
SchedulerTest01-Child1: performing spawn of second child
SchedulerTest01-Child1: spawn of second child returned pid = 5
SchedulerTest01-Child1: performing first wait
SchedulerTest01-Child1-Child1: started
SchedulerTest01-Child1-Child1: quitting
SchedulerTest01-Child1: exit status for child 4 is -3
SchedulerTest01-Child1: performing second wait
SchedulerTest01-Child1-Child2: started
SchedulerTest01-Child1-Child2: quitting
SchedulerTest01-Child1: exit status for child 5 is -3
SchedulerTest01: exit status for child 3 is 1
All processes completed.

SchedulerTest02: started
SchedulerTest02: after spawn of child with pid 3
SchedulerTest02: waiting for child process
SchedulerTest02-Child1: started
SchedulerTest02-Child1: performing spawn of first child
SchedulerTest02-Child1-Child1: started
SchedulerTest02-Child1-Child1: quitting
SchedulerTest02-Child1: spawn of first child returned pid = 4
SchedulerTest02-Child1: performing spawn of second child
SchedulerTest02-Child1-Child2: started
SchedulerTest02-Child1-Child2: quitting
SchedulerTest02-Child1: spawn of second child returned pid = 5
SchedulerTest02-Child1: performing first join.
SchedulerTest02-Child1: exit status for child 4 is -3
SchedulerTest02-Child1: performing second join.
SchedulerTest02-Child1: exit status for child 5 is -3
SchedulerTest02: exit status for child 3 is -3
All processes completed.

SchedulerTest03: started
SchedulerTest03: signaling first child
SchedulerTest03-Child1: started
SchedulerTest03-Child1: quitting
SchedulerTest03: exit status for child 3 is -5
All processes completed.

SchedulerTest04: started
SchedulerTest04: after spawn of child 3
SchedulerTest04: after spawn of child 4
SchedulerTest04: after spawn of child 5
SchedulerTest04: after spawn of child 6
SchedulerTest04: after spawn of child 7
SchedulerTest04: after spawn of child 8
SchedulerTest04: after spawn of child 9
SchedulerTest04: after spawn of child 10
SchedulerTest04: after spawn of child 11
SchedulerTest04: after spawn of child 12
SchedulerTest04: after spawn of child 13
SchedulerTest04: after spawn of child 14
SchedulerTest04: after spawn of child 15
SchedulerTest04: after spawn of child 16
SchedulerTest04: after spawn of child 17
SchedulerTest04: after spawn of child 18
SchedulerTest04: after spawn of child 19
SchedulerTest04: after spawn of child 20
SchedulerTest04: after spawn of child 21
SchedulerTest04: after spawn of child 22
SchedulerTest04: after spawn of child 23
SchedulerTest04: after spawn of child 24
SchedulerTest04: after spawn of child 25
SchedulerTest04: after spawn of child 26
SchedulerTest04: after spawn of child 27
SchedulerTest04: after spawn of child 28
SchedulerTest04: after spawn of child 29
SchedulerTest04: after spawn of child 30
SchedulerTest04: after spawn of child 31
SchedulerTest04: after spawn of child 32
SchedulerTest04: after spawn of child 33
SchedulerTest04: after spawn of child 34
SchedulerTest04: after spawn of child 35
SchedulerTest04: after spawn of child 36
SchedulerTest04: after spawn of child 37
SchedulerTest04: after spawn of child 38
SchedulerTest04: after spawn of child 39
SchedulerTest04: after spawn of child 40
SchedulerTest04: after spawn of child 41
SchedulerTest04: after spawn of child 42
SchedulerTest04: after spawn of child 43
SchedulerTest04: after spawn of child 44
SchedulerTest04: after spawn of child 45
SchedulerTest04: after spawn of child 46
SchedulerTest04: after spawn of child 47
SchedulerTest04: after spawn of child 48
SchedulerTest04: after spawn of child 49
SchedulerTest04: after spawn of child 50
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
50      2        3         READY         0        0        SchedulerTest04-Child48
1       -1       0         READY         0        0        watchdog
2       -1       5         RUNNING       48       0        Scheduler
3       2        3         READY         0        0        SchedulerTest04-Child1
4       2        3         READY         0        0        SchedulerTest04-Child2
5       2        3         READY         0        0        SchedulerTest04-Child3
6       2        3         READY         0        0        SchedulerTest04-Child4
7       2        3         READY         0        0        SchedulerTest04-Child5
8       2        3         READY         0        0        SchedulerTest04-Child6
9       2        3         READY         0        0        SchedulerTest04-Child7
10      2        3         READY         0        0        SchedulerTest04-Child8
11      2        3         READY         0        0        SchedulerTest04-Child9
12      2        3         READY         0        0        SchedulerTest04-Child10
13      2        3         READY         0        0        SchedulerTest04-Child11
14      2        3         READY         0        0        SchedulerTest04-Child12
15      2        3         READY         0        0        SchedulerTest04-Child13
16      2        3         READY         0        0        SchedulerTest04-Child14
17      2        3         READY         0        0        SchedulerTest04-Child15
18      2        3         READY         0        0        SchedulerTest04-Child16
19      2        3         READY         0        0        SchedulerTest04-Child17
20      2        3         READY         0        0        SchedulerTest04-Child18
21      2        3         READY         0        0        SchedulerTest04-Child19
22      2        3         READY         0        0        SchedulerTest04-Child20
23      2        3         READY         0        0        SchedulerTest04-Child21
24      2        3         READY         0        0        SchedulerTest04-Child22
25      2        3         READY         0        0        SchedulerTest04-Child23
26      2        3         READY         0        0        SchedulerTest04-Child24
27      2        3         READY         0        0        SchedulerTest04-Child25
28      2        3         READY         0        0        SchedulerTest04-Child26
29      2        3         READY         0        0        SchedulerTest04-Child27
30      2        3         READY         0        0        SchedulerTest04-Child28
31      2        3         READY         0        0        SchedulerTest04-Child29
32      2        3         READY         0        0        SchedulerTest04-Child30
33      2        3         READY         0        0        SchedulerTest04-Child31
34      2        3         READY         0        0        SchedulerTest04-Child32
35      2        3         READY         0        0        SchedulerTest04-Child33
36      2        3         READY         0        0        SchedulerTest04-Child34
37      2        3         READY         0        0        SchedulerTest04-Child35
38      2        3         READY         0        0        SchedulerTest04-Child36
39      2        3         READY         0        0        SchedulerTest04-Child37
40      2        3         READY         0        0        SchedulerTest04-Child38
41      2        3         READY         0        0        SchedulerTest04-Child39
42      2        3         READY         0        0        SchedulerTest04-Child40
43      2        3         READY         0        0        SchedulerTest04-Child41
44      2        3         READY         0        0        SchedulerTest04-Child42
45      2        3         READY         0        0        SchedulerTest04-Child43
46      2        3         READY         0        0        SchedulerTest04-Child44
47      2        3         READY         0        0        SchedulerTest04-Child45
48      2        3         READY         0        0        SchedulerTest04-Child46
49      2        3         READY         0        0        SchedulerTest04-Child47
SchedulerTest04-Child1: started
SchedulerTest04-Child1: quitting
SchedulerTest04: exit status for child 3 is -3
SchedulerTest04-Child2: started
SchedulerTest04-Child2: quitting
SchedulerTest04: exit status for child 4 is -3
SchedulerTest04-Child3: started
SchedulerTest04-Child3: quitting
SchedulerTest04: exit status for child 5 is -3
SchedulerTest04-Child4: started
SchedulerTest04-Child4: quitting
SchedulerTest04: exit status for child 6 is -3
SchedulerTest04-Child5: started
SchedulerTest04-Child5: quitting
SchedulerTest04: exit status for child 7 is -3
SchedulerTest04-Child6: started
SchedulerTest04-Child6: quitting
SchedulerTest04: exit status for child 8 is -3
SchedulerTest04-Child7: started
SchedulerTest04-Child7: quitting
SchedulerTest04: exit status for child 9 is -3
SchedulerTest04-Child8: started
SchedulerTest04-Child8: quitting
SchedulerTest04: exit status for child 10 is -3
SchedulerTest04-Child9: started
SchedulerTest04-Child9: quitting
SchedulerTest04: exit status for child 11 is -3
SchedulerTest04-Child10: started
SchedulerTest04-Child10: quitting
SchedulerTest04: exit status for child 12 is -3
SchedulerTest04-Child11: started
SchedulerTest04-Child11: quitting
SchedulerTest04: exit status for child 13 is -3
SchedulerTest04-Child12: started
SchedulerTest04-Child12: quitting
SchedulerTest04: exit status for child 14 is -3
SchedulerTest04-Child13: started
SchedulerTest04-Child13: quitting
SchedulerTest04: exit status for child 15 is -3
SchedulerTest04-Child14: started
SchedulerTest04-Child14: quitting
SchedulerTest04: exit status for child 16 is -3
SchedulerTest04-Child15: started
SchedulerTest04-Child15: quitting
SchedulerTest04: exit status for child 17 is -3
SchedulerTest04-Child16: started
SchedulerTest04-Child16: quitting
SchedulerTest04: exit status for child 18 is -3
SchedulerTest04-Child17: started
SchedulerTest04-Child17: quitting
SchedulerTest04: exit status for child 19 is -3
SchedulerTest04-Child18: started
SchedulerTest04-Child18: quitting
SchedulerTest04: exit status for child 20 is -3
SchedulerTest04-Child19: started
SchedulerTest04-Child19: quitting
SchedulerTest04: exit status for child 21 is -3
SchedulerTest04-Child20: started
SchedulerTest04-Child20: quitting
SchedulerTest04: exit status for child 22 is -3
SchedulerTest04-Child21: started
SchedulerTest04-Child21: quitting
SchedulerTest04: exit status for child 23 is -3
SchedulerTest04-Child22: started
SchedulerTest04-Child22: quitting
SchedulerTest04: exit status for child 24 is -3
SchedulerTest04-Child23: started
SchedulerTest04-Child23: quitting
SchedulerTest04: exit status for child 25 is -3
SchedulerTest04-Child24: started
SchedulerTest04-Child24: quitting
SchedulerTest04: exit status for child 26 is -3
SchedulerTest04-Child25: started
SchedulerTest04-Child25: quitting
SchedulerTest04: exit status for child 27 is -3
SchedulerTest04-Child26: started
SchedulerTest04-Child26: quitting
SchedulerTest04: exit status for child 28 is -3
SchedulerTest04-Child27: started
SchedulerTest04-Child27: quitting
SchedulerTest04: exit status for child 29 is -3
SchedulerTest04-Child28: started
SchedulerTest04-Child28: quitting
SchedulerTest04: exit status for child 30 is -3
SchedulerTest04-Child29: started
SchedulerTest04-Child29: quitting
SchedulerTest04: exit status for child 31 is -3
SchedulerTest04-Child30: started
SchedulerTest04-Child30: quitting
SchedulerTest04: exit status for child 32 is -3
SchedulerTest04-Child31: started
SchedulerTest04-Child31: quitting
SchedulerTest04: exit status for child 33 is -3
SchedulerTest04-Child32: started
SchedulerTest04-Child32: quitting
SchedulerTest04: exit status for child 34 is -3
SchedulerTest04-Child33: started
SchedulerTest04-Child33: quitting
SchedulerTest04: exit status for child 35 is -3
SchedulerTest04-Child34: started
SchedulerTest04-Child34: quitting
SchedulerTest04: exit status for child 36 is -3
SchedulerTest04-Child35: started
SchedulerTest04-Child35: quitting
SchedulerTest04: exit status for child 37 is -3
SchedulerTest04-Child36: started
SchedulerTest04-Child36: quitting
SchedulerTest04: exit status for child 38 is -3
SchedulerTest04-Child37: started
SchedulerTest04-Child37: quitting
SchedulerTest04: exit status for child 39 is -3
SchedulerTest04-Child38: started
SchedulerTest04-Child38: quitting
SchedulerTest04: exit status for child 40 is -3
SchedulerTest04-Child39: started
SchedulerTest04-Child39: quitting
SchedulerTest04: exit status for child 41 is -3
SchedulerTest04-Child40: started
SchedulerTest04-Child40: quitting
SchedulerTest04: exit status for child 42 is -3
SchedulerTest04-Child41: started
SchedulerTest04-Child41: quitting
SchedulerTest04: exit status for child 43 is -3
SchedulerTest04-Child42: started
SchedulerTest04-Child42: quitting
SchedulerTest04: exit status for child 44 is -3
SchedulerTest04-Child43: started
SchedulerTest04-Child43: quitting
SchedulerTest04: exit status for child 45 is -3
SchedulerTest04-Child44: started
SchedulerTest04-Child44: quitting
SchedulerTest04: exit status for child 46 is -3
SchedulerTest04-Child45: started
SchedulerTest04-Child45: quitting
SchedulerTest04: exit status for child 47 is -3
SchedulerTest04-Child46: started
SchedulerTest04-Child46: quitting
SchedulerTest04: exit status for child 48 is -3
SchedulerTest04-Child47: started
SchedulerTest04-Child47: quitting
SchedulerTest04: exit status for child 49 is -3
SchedulerTest04-Child48: started
SchedulerTest04-Child48: quitting
SchedulerTest04: exit status for child 50 is -3
SchedulerTest04: after spawn of child 53
SchedulerTest04: after spawn of child 54
SchedulerTest04: after spawn of child 55
SchedulerTest04: after spawn of child 56
SchedulerTest04: after spawn of child 57
SchedulerTest04: after spawn of child 58
SchedulerTest04: after spawn of child 59
SchedulerTest04: after spawn of child 60
SchedulerTest04: after spawn of child 61
SchedulerTest04: after spawn of child 62
SchedulerTest04: after spawn of child 63
SchedulerTest04: after spawn of child 64
SchedulerTest04: after spawn of child 65
SchedulerTest04: after spawn of child 66
SchedulerTest04: after spawn of child 67
SchedulerTest04: after spawn of child 68
SchedulerTest04: after spawn of child 69
SchedulerTest04: after spawn of child 70
SchedulerTest04: after spawn of child 71
SchedulerTest04: after spawn of child 72
SchedulerTest04: after spawn of child 73
SchedulerTest04: after spawn of child 74
SchedulerTest04: after spawn of child 75
SchedulerTest04: after spawn of child 76
SchedulerTest04: after spawn of child 77
SchedulerTest04: after spawn of child 78
SchedulerTest04: after spawn of child 79
SchedulerTest04: after spawn of child 80
SchedulerTest04: after spawn of child 81
SchedulerTest04: after spawn of child 82
SchedulerTest04: after spawn of child 83
SchedulerTest04: after spawn of child 84
SchedulerTest04: after spawn of child 85
SchedulerTest04: after spawn of child 86
SchedulerTest04: after spawn of child 87
SchedulerTest04: after spawn of child 88
SchedulerTest04: after spawn of child 89
SchedulerTest04: after spawn of child 90
SchedulerTest04: after spawn of child 91
SchedulerTest04: after spawn of child 92
SchedulerTest04: after spawn of child 93
SchedulerTest04: after spawn of child 94
SchedulerTest04: after spawn of child 95
SchedulerTest04: after spawn of child 96
SchedulerTest04: after spawn of child 97
SchedulerTest04: after spawn of child 98
SchedulerTest04: after spawn of child 99
SchedulerTest04: after spawn of child 100
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
100     2        3         READY         0        0        SchedulerTest04-Child96
1       -1       0         READY         0        0        watchdog
2       -1       5         RUNNING       48       183      Scheduler
53      2        3         READY         0        0        SchedulerTest04-Child49
54      2        3         READY         0        0        SchedulerTest04-Child50
55      2        3         READY         0        0        SchedulerTest04-Child51
56      2        3         READY         0        0        SchedulerTest04-Child52
57      2        3         READY         0        0        SchedulerTest04-Child53
58      2        3         READY         0        0        SchedulerTest04-Child54
59      2        3         READY         0        0        SchedulerTest04-Child55
60      2        3         READY         0        0        SchedulerTest04-Child56
61      2        3         READY         0        0        SchedulerTest04-Child57
62      2        3         READY         0        0        SchedulerTest04-Child58
63      2        3         READY         0        0        SchedulerTest04-Child59
64      2        3         READY         0        0        SchedulerTest04-Child60
65      2        3         READY         0        0        SchedulerTest04-Child61
66      2        3         READY         0        0        SchedulerTest04-Child62
67      2        3         READY         0        0        SchedulerTest04-Child63
68      2        3         READY         0        0        SchedulerTest04-Child64
69      2        3         READY         0        0        SchedulerTest04-Child65
70      2        3         READY         0        0        SchedulerTest04-Child66
71      2        3         READY         0        0        SchedulerTest04-Child67
72      2        3         READY         0        0        SchedulerTest04-Child68
73      2        3         READY         0        0        SchedulerTest04-Child69
74      2        3         READY         0        0        SchedulerTest04-Child70
75      2        3         READY         0        0        SchedulerTest04-Child71
76      2        3         READY         0        0        SchedulerTest04-Child72
77      2        3         READY         0        0        SchedulerTest04-Child73
78      2        3         READY         0        0        SchedulerTest04-Child74
79      2        3         READY         0        0        SchedulerTest04-Child75
80      2        3         READY         0        0        SchedulerTest04-Child76
81      2        3         READY         0        0        SchedulerTest04-Child77
82      2        3         READY         0        0        SchedulerTest04-Child78
83      2        3         READY         0        0        SchedulerTest04-Child79
84      2        3         READY         0        0        SchedulerTest04-Child80
85      2        3         READY         0        0        SchedulerTest04-Child81
86      2        3         READY         0        0        SchedulerTest04-Child82
87      2        3         READY         0        0        SchedulerTest04-Child83
88      2        3         READY         0        0        SchedulerTest04-Child84
89      2        3         READY         0        0        SchedulerTest04-Child85
90      2        3         READY         0        0        SchedulerTest04-Child86
91      2        3         READY         0        0        SchedulerTest04-Child87
92      2        3         READY         0        0        SchedulerTest04-Child88
93      2        3         READY         0        0        SchedulerTest04-Child89
94      2        3         READY         0        0        SchedulerTest04-Child90
95      2        3         READY         0        0        SchedulerTest04-Child91
96      2        3         READY         0        0        SchedulerTest04-Child92
97      2        3         READY         0        0        SchedulerTest04-Child93
98      2        3         READY         0        0        SchedulerTest04-Child94
99      2        3         READY         0        0        SchedulerTest04-Child95
SchedulerTest04-Child49: started
SchedulerTest04-Child49: quitting
SchedulerTest04: exit status for child 53 is -3
SchedulerTest04-Child50: started
SchedulerTest04-Child50: quitting
SchedulerTest04: exit status for child 54 is -3
SchedulerTest04-Child51: started
SchedulerTest04-Child51: quitting
SchedulerTest04: exit status for child 55 is -3
SchedulerTest04-Child52: started
SchedulerTest04-Child52: quitting
SchedulerTest04: exit status for child 56 is -3
SchedulerTest04-Child53: started
SchedulerTest04-Child53: quitting
SchedulerTest04: exit status for child 57 is -3
SchedulerTest04-Child54: started
SchedulerTest04-Child54: quitting
SchedulerTest04: exit status for child 58 is -3
SchedulerTest04-Child55: started
SchedulerTest04-Child55: quitting
SchedulerTest04: exit status for child 59 is -3
SchedulerTest04-Child56: started
SchedulerTest04-Child56: quitting
SchedulerTest04: exit status for child 60 is -3
SchedulerTest04-Child57: started
SchedulerTest04-Child57: quitting
SchedulerTest04: exit status for child 61 is -3
SchedulerTest04-Child58: started
SchedulerTest04-Child58: quitting
SchedulerTest04: exit status for child 62 is -3
SchedulerTest04-Child59: started
SchedulerTest04-Child59: quitting
SchedulerTest04: exit status for child 63 is -3
SchedulerTest04-Child60: started
SchedulerTest04-Child60: quitting
SchedulerTest04: exit status for child 64 is -3
SchedulerTest04-Child61: started
SchedulerTest04-Child61: quitting
SchedulerTest04: exit status for child 65 is -3
SchedulerTest04-Child62: started
SchedulerTest04-Child62: quitting
SchedulerTest04: exit status for child 66 is -3
SchedulerTest04-Child63: started
SchedulerTest04-Child63: quitting
SchedulerTest04: exit status for child 67 is -3
SchedulerTest04-Child64: started
SchedulerTest04-Child64: quitting
SchedulerTest04: exit status for child 68 is -3
SchedulerTest04-Child65: started
SchedulerTest04-Child65: quitting
SchedulerTest04: exit status for child 69 is -3
SchedulerTest04-Child66: started
SchedulerTest04-Child66: quitting
SchedulerTest04: exit status for child 70 is -3
SchedulerTest04-Child67: started
SchedulerTest04-Child67: quitting
SchedulerTest04: exit status for child 71 is -3
SchedulerTest04-Child68: started
SchedulerTest04-Child68: quitting
SchedulerTest04: exit status for child 72 is -3
SchedulerTest04-Child69: started
SchedulerTest04-Child69: quitting
SchedulerTest04: exit status for child 73 is -3
SchedulerTest04-Child70: started
SchedulerTest04-Child70: quitting
SchedulerTest04: exit status for child 74 is -3
SchedulerTest04-Child71: started
SchedulerTest04-Child71: quitting
SchedulerTest04: exit status for child 75 is -3
SchedulerTest04-Child72: started
SchedulerTest04-Child72: quitting
SchedulerTest04: exit status for child 76 is -3
SchedulerTest04-Child73: started
SchedulerTest04-Child73: quitting
SchedulerTest04: exit status for child 77 is -3
SchedulerTest04-Child74: started
SchedulerTest04-Child74: quitting
SchedulerTest04: exit status for child 78 is -3
SchedulerTest04-Child75: started
SchedulerTest04-Child75: quitting
SchedulerTest04: exit status for child 79 is -3
SchedulerTest04-Child76: started
SchedulerTest04-Child76: quitting
SchedulerTest04: exit status for child 80 is -3
SchedulerTest04-Child77: started
SchedulerTest04-Child77: quitting
SchedulerTest04: exit status for child 81 is -3
SchedulerTest04-Child78: started
SchedulerTest04-Child78: quitting
SchedulerTest04: exit status for child 82 is -3
SchedulerTest04-Child79: started
SchedulerTest04-Child79: quitting
SchedulerTest04: exit status for child 83 is -3
SchedulerTest04-Child80: started
SchedulerTest04-Child80: quitting
SchedulerTest04: exit status for child 84 is -3
SchedulerTest04-Child81: started
SchedulerTest04-Child81: quitting
SchedulerTest04: exit status for child 85 is -3
SchedulerTest04-Child82: started
SchedulerTest04-Child82: quitting
SchedulerTest04: exit status for child 86 is -3
SchedulerTest04-Child83: started
SchedulerTest04-Child83: quitting
SchedulerTest04: exit status for child 87 is -3
SchedulerTest04-Child84: started
SchedulerTest04-Child84: quitting
SchedulerTest04: exit status for child 88 is -3
SchedulerTest04-Child85: started
SchedulerTest04-Child85: quitting
SchedulerTest04: exit status for child 89 is -3
SchedulerTest04-Child86: started
SchedulerTest04-Child86: quitting
SchedulerTest04: exit status for child 90 is -3
SchedulerTest04-Child87: started
SchedulerTest04-Child87: quitting
SchedulerTest04: exit status for child 91 is -3
SchedulerTest04-Child88: started
SchedulerTest04-Child88: quitting
SchedulerTest04: exit status for child 92 is -3
SchedulerTest04-Child89: started
SchedulerTest04-Child89: quitting
SchedulerTest04: exit status for child 93 is -3
SchedulerTest04-Child90: started
SchedulerTest04-Child90: quitting
SchedulerTest04: exit status for child 94 is -3
SchedulerTest04-Child91: started
SchedulerTest04-Child91: quitting
SchedulerTest04: exit status for child 95 is -3
SchedulerTest04-Child92: started
SchedulerTest04-Child92: quitting
SchedulerTest04: exit status for child 96 is -3
SchedulerTest04-Child93: started
SchedulerTest04-Child93: quitting
SchedulerTest04: exit status for child 97 is -3
SchedulerTest04-Child94: started
SchedulerTest04-Child94: quitting
SchedulerTest04: exit status for child 98 is -3
SchedulerTest04-Child95: started
SchedulerTest04-Child95: quitting
SchedulerTest04: exit status for child 99 is -3
SchedulerTest04-Child96: started
SchedulerTest04-Child96: quitting
SchedulerTest04: exit status for child 100 is -3
All processes completed.

SchedulerTest05: started
SchedulerTest05: after spawn of child with pid 3
SchedulerTest05: after spawn of child with pid 4
SchedulerTest05: after spawn of child with pid 5
SchedulerTest05: after spawn of child with pid 6
SchedulerTest05-Child1: started
SchedulerTest05-Child1: started, child number is 1
SchedulerTest05-Child2: started
SchedulerTest05-Child2: started, child number is 2
SchedulerTest05-Child3: started
SchedulerTest05-Child3: started, child number is 3
SchedulerTest05-Child4: started
SchedulerTest05-Child4: started, child number is 4
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    4        0        Scheduler
3       2        3         READY         0        722      SchedulerTest05-Child1
4       2        3         READY         0        736      SchedulerTest05-Child2
5       2        3         READY         0        726      SchedulerTest05-Child3
6       2        3         RUNNING       0        632      SchedulerTest05-Child4
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    4        0        Scheduler
3       2        3         READY         0        1903     SchedulerTest05-Child1
4       2        3         READY         0        1916     SchedulerTest05-Child2
5       2        3         READY         0        1916     SchedulerTest05-Child3
6       2        3         RUNNING       0        1837     SchedulerTest05-Child4
SchedulerTest05-Child1: exiting, pid = 3
SchedulerTest05: exit status for child 3 is -3
SchedulerTest05-Child2: exiting, pid = 4
SchedulerTest05: exit status for child 4 is -4
SchedulerTest05-Child3: exiting, pid = 5
SchedulerTest05: exit status for child 5 is -5
SchedulerTest05-Child4: exiting, pid = 6
SchedulerTest05: exit status for child 6 is -6
SchedulerTest05: after spawn of child with pid 7
SchedulerTest05: after spawn of child with pid 8
SchedulerTest05: after spawn of child with pid 9
SchedulerTest05: after spawn of child with pid 10
SchedulerTest05-Child5: started
SchedulerTest05-Child5: started, child number is 5
SchedulerTest05-Child6: started
SchedulerTest05-Child6: started, child number is 6
SchedulerTest05-Child7: started
SchedulerTest05-Child7: started, child number is 7
SchedulerTest05-Child8: started
SchedulerTest05-Child8: started, child number is 8
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    4        15       Scheduler
7       2        3         READY         0        732      SchedulerTest05-Child5
8       2        3         READY         0        730      SchedulerTest05-Child6
9       2        3         READY         0        724      SchedulerTest05-Child7
10      2        3         RUNNING       0        630      SchedulerTest05-Child8
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    4        15       Scheduler
7       2        3         READY         0        1907     SchedulerTest05-Child5
8       2        3         READY         0        1914     SchedulerTest05-Child6
9       2        3         READY         0        1894     SchedulerTest05-Child7
10      2        3         RUNNING       0        1857     SchedulerTest05-Child8
SchedulerTest05-Child5: exiting, pid = 7
SchedulerTest05: exit status for child 7 is -7
SchedulerTest05-Child6: exiting, pid = 8
SchedulerTest05: exit status for child 8 is -8
SchedulerTest05-Child7: exiting, pid = 9
SchedulerTest05: exit status for child 9 is -9
SchedulerTest05-Child8: exiting, pid = 10
SchedulerTest05: exit status for child 10 is -10
SchedulerTest05: after spawn of child with pid 11
SchedulerTest05: after spawn of child with pid 12
SchedulerTest05: after spawn of child with pid 13
SchedulerTest05: after spawn of child with pid 14
SchedulerTest05-Child9: started
SchedulerTest05-Child9: started, child number is 9
SchedulerTest05-Child10: started
SchedulerTest05-Child10: started, child number is 10
SchedulerTest05-Child11: started
SchedulerTest05-Child11: started, child number is 11
SchedulerTest05-Child12: started
SchedulerTest05-Child12: started, child number is 12
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    4        15       Scheduler
11      2        3         READY         0        731      SchedulerTest05-Child9
12      2        3         READY         0        726      SchedulerTest05-Child10
13      2        3         READY         0        722      SchedulerTest05-Child11
14      2        3         RUNNING       0        633      SchedulerTest05-Child12
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    4        15       Scheduler
11      2        3         READY         0        1910     SchedulerTest05-Child9
12      2        3         READY         0        1907     SchedulerTest05-Child10
13      2        3         READY         0        1904     SchedulerTest05-Child11
14      2        3         RUNNING       0        1864     SchedulerTest05-Child12
SchedulerTest05-Child9: exiting, pid = 11
SchedulerTest05: exit status for child 11 is -11
SchedulerTest05-Child10: exiting, pid = 12
SchedulerTest05: exit status for child 12 is -12
SchedulerTest05-Child11: exiting, pid = 13
SchedulerTest05: exit status for child 13 is -13
SchedulerTest05-Child12: exiting, pid = 14
SchedulerTest05: exit status for child 14 is -14
All processes completed.

SchedulerTest06: started
SchedulerTest06: after spawn of child with pid 3
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         RUNNING       1        0        Scheduler
3       2        3         READY         0        0        SchedulerTest06-Child1
SchedulerTest06: joining child process
SchedulerTest06-Child1: started
SchedulerTest06-Child1: performing spawn of first child
SchedulerTest06-Child1: spawn of first child returned pid = 4
SchedulerTest06-Child1: performing spawn of second child
SchedulerTest06-Child1: spawn of first child returned pid = 5
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    1        0        Scheduler
3       2        3         RUNNING       2        0        SchedulerTest06-Child1
4       3        2         READY         0        0        SchedulerTest06-Child1-Child1
5       3        2         READY         0        0        SchedulerTest06-Child1-Child2
SchedulerTest06-Child1: signaling first child
SchedulerTest06-Child1-Child1: started
SchedulerTest06-Child1-Child1: performing spawn of child
SchedulerTest06-Child1-Child2: started
SchedulerTest06-Child1-Child2: performing spawn of child
SchedulerTest06-Child1-Child1: spawn of child returned pid = 6
SchedulerTest06-Child1-Child2: spawn of child returned pid = 7
SchedulerTest06-Child1-Child1-Child1: started
SchedulerTest06-Child1-Child1-Child1: quitting
SchedulerTest06-Child1-Child1-Child1: exit status for child -5 is -3
SchedulerTest06-Child1: after joining first child, status = -5
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    1        0        Scheduler
3       2        3         RUNNING       1        0        SchedulerTest06-Child1
4       3        2         QUIT          0        0        SchedulerTest06-Child1-Child1
5       3        2         WAIT BLOCK    1        0        SchedulerTest06-Child1-Child2
7       5        1         READY         0        0        SchedulerTest06-Child1-Child2-Child1
SchedulerTest06-Child1: signaling second child
SchedulerTest06-Child1-Child2-Child1: started
SchedulerTest06-Child1-Child2-Child1: quitting
SchedulerTest06-Child1-Child2-Child1: exit status for child -5 is -3
SchedulerTest06-Child1: after joining second child, status = -5
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    1        0        Scheduler
3       2        3         RUNNING       0        0        SchedulerTest06-Child1
4       3        2         QUIT          0        0        SchedulerTest06-Child1-Child1
5       3        2         QUIT          0        0        SchedulerTest06-Child1-Child2
SchedulerTest06-Child1: performing joins
SchedulerTest06-Child1: exit status for child 4 is -5
SchedulerTest06-Child1: exit status for child 5 is -5
SchedulerTest06: exit status for child 3 is -3
All processes completed.

SchedulerTest07: started
SchedulerTest07: waiting for child process
SchedulerTest07-Child1: started
SchedulerTest07-Child1: performing spawn of child
SchedulerTest07-Child1-Child1: started
SchedulerTest07-Child1-Child1: quitting
SchedulerTest07-Child1: spawn of child returned pid = 4
SchedulerTest07-Child1: exit status for child 4 is -3
SchedulerTest07: exit status for child 3 is -3
All processes completed.

SchedulerTest08: started
SchedulerTest08: after spawn of child with pid 3
SchedulerTest08: waiting for child process
SchedulerTest08-Child1: started
SchedulerTest08-Child1: performing spawn of first child
SchedulerTest08-Child1: spawn of first child returned pid = 4
SchedulerTest08-Child1: performing spawn of second child
SchedulerTest08-Child1: spawn of second child returned pid = 5
SchedulerTest08-Child1: performing first wait
SchedulerTest08-Child1-Child1: started
SchedulerTest08-Child1-Child1: quitting
SchedulerTest08-Child1: exit status for child 4 is -3
SchedulerTest08-Child1: performing second wait
SchedulerTest08-Child1-Child2: started
SchedulerTest08-Child1-Child2: quitting
SchedulerTest08-Child1: exit status for child 5 is -3
SchedulerTest08: exit status for child 3 is 1
All processes completed.

SchedulerTest09: started
SchedulerTest09: after spawn of child with pid 3
quit(): Process with active children attempting to quit
THREADS Halted: Error code 1.

SchedulerTest10: started
SchedulerTest10: after spawn of child with pid 3
SchedulerTest10: waiting for child process
SchedulerTest10-Child1: started
SchedulerTest10-Child1: joining parent process
join: process attempted to join parent.
THREADS Halted: Error code 2.

SchedulerTest11: started
Kernel mode expected, but function called in user mode.
THREADS Halted: Error code 1.

SchedulerTest12: started
SchedulerTest12: after spawn of child with pid 3
SchedulerTest12: waiting for child process
SchedulerTest12-Child1: started
SchedulerTest12-Child1: quitting
SchedulerTest12: exit status for child 3 is -3
Kernel mode expected, but function called in user mode.
THREADS Halted: Error code 1.

SchedulerTest13: started
SchedulerTest13: after spawn of child with pid 3
SchedulerTest13: after spawn of child with pid 4
SchedulerTest13: waiting for child process
SchedulerTest13-Child1: started
SchedulerTest13-Child1: performing spawn of child
SchedulerTest13-Child1: spawn of child returned pid = 5
SchedulerTest13-Child2: started
SchedulerTest13-Child2: signaling 5.
SchedulerTest13-Child2: joining 5.
SchedulerTest13-Child1-Child1: started
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    2        0        Scheduler
3       2        4         WAIT BLOCK    1        0        SchedulerTest13-Child1
4       2        3         JOIN BLOCK    0        0        SchedulerTest13-Child2
5       3        2         RUNNING       0        0        SchedulerTest13-Child1-Child1
SchedulerTest13-Child1: exit status for child 5 is -5
SchedulerTest13: exit status for child 3 is -3
SchedulerTest13: waiting for child process
SchedulerTest13-Child2: after joining child, pid 5, status = -5
SchedulerTest13: exit status for child 4 is 5
All processes completed.

SchedulerTest14: started
SchedulerTest14: after spawn of child with pid 3
SchedulerTest14: after spawn of child with pid 4
SchedulerTest14: waiting for child process
SchedulerTest14-Child1: started
SchedulerTest14-Child1: performing spawn of child
SchedulerTest14-Child1: spawn of child returned pid = 5
SchedulerTest14-Child2: started
SchedulerTest14-Child2: signaling 3.
SchedulerTest14-Child2: joining 3.
SchedulerTest14-Child1-Child1: started
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    2        0        Scheduler
3       2        4         WAIT BLOCK    1        0        SchedulerTest14-Child1
4       2        3         JOIN BLOCK    0        0        SchedulerTest14-Child2
5       3        2         RUNNING       0        0        SchedulerTest14-Child1-Child1
SchedulerTest14-Child1: process was signaled during wait
SchedulerTest14: exit status for child 3 is -5
SchedulerTest14: waiting for child process
SchedulerTest14-Child2: after joining child, pid 3, status = -5
SchedulerTest14: exit status for child 4 is 5
All processes completed.

SchedulerTest15: started
SchedulerTest15: after spawn of child with pid 3
SchedulerTest15: after spawn of child with pid 4
SchedulerTest15: waiting for child process
SchedulerTest15-Child1: started
SchedulerTest15-Child1: performing spawn of child
SchedulerTest15-Child1: spawn of child returned pid = 5
SchedulerTest15-Child1: joining 5
SchedulerTest15-Child2: started
SchedulerTest15-Child2: joining 5
SchedulerTest15-Child1-Child1: started
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    2        0        Scheduler
3       2        4         JOIN BLOCK    1        0        SchedulerTest15-Child1
4       2        3         JOIN BLOCK    0        0        SchedulerTest15-Child2
5       3        2         RUNNING       0        0        SchedulerTest15-Child1-Child1
SchedulerTest15-Child1: after joining child, pid 5, status = -3
SchedulerTest15-Child1: waiting for 5
SchedulerTest15-Child1: exit status for child 5 is -3
SchedulerTest15: exit status for child 3 is -3
SchedulerTest15: waiting for child process
SchedulerTest15-Child2: after joining child, pid 5, status = -3
SchedulerTest15: exit status for child 4 is 5
All processes completed.

SchedulerTest16: started
SchedulerTest16: performing spawn of child
SchedulerTest16: after spawn of child with pid 3
SchedulerTest16: performing spawn of child
SchedulerTest16: after spawn of child with pid 4
SchedulerTest16: performing spawn of child
SchedulerTest16: after spawn of child with pid 5
SchedulerTest16: performing spawn of child
SchedulerTest16: after spawn of child with pid 6
SchedulerTest16: performing spawn of child
SchedulerTest16: after spawn of child with pid 7
SchedulerTest16: waiting for child process
SchedulerTest16-Child1: k_getPid test PASSED by returning 3
SchedulerTest16: exit status for child 3 is -2
SchedulerTest16: waiting for child process
SchedulerTest16-Child2: k_getPid test PASSED by returning 4
SchedulerTest16: exit status for child 4 is -2
SchedulerTest16: waiting for child process
SchedulerTest16-Child3: k_getPid test PASSED by returning 5
SchedulerTest16: exit status for child 5 is -2
SchedulerTest16: waiting for child process
SchedulerTest16-Child4: k_getPid test PASSED by returning 6
SchedulerTest16: exit status for child 6 is -2
SchedulerTest16: waiting for child process
SchedulerTest16-Child5: k_getPid test PASSED by returning 7
SchedulerTest16: exit status for child 7 is -2
All processes completed.

SchedulerTest17: started
SchedulerTest17: performing spawn of child
SchedulerTest17: performing spawn of child
SchedulerTest17: after spawn of child with pid 4
SchedulerTest17: performing spawn of child
SchedulerTest17: after spawn of child with pid 5
SchedulerTest17: performing spawn of child
SchedulerTest17: after spawn of child with pid 6
SchedulerTest17: performing spawn of child
SchedulerTest17: after spawn of child with pid 7
SchedulerTest17: performing spawn of child
SchedulerTest17: after spawn of child with pid 8
SchedulerTest17: performing spawn of child
SchedulerTest17: after spawn of child with pid 9
SchedulerTest17: performing spawn of child
SchedulerTest17: after spawn of child with pid 10
SchedulerTest17: performing spawn of child
SchedulerTest17: after spawn of child with pid 11
SchedulerTest17: performing spawn of child
SchedulerTest17: after spawn of child with pid 12
SchedulerTest17: performing spawn of child
SchedulerTest17: after spawn of child with pid 13
SchedulerTest17: waiting for child process
SchedulerTest17-Child2: started
SchedulerTest17-Child2: joining 3
SchedulerTest17-Child3: started
SchedulerTest17-Child3: joining 3
SchedulerTest17-Child4: started
SchedulerTest17-Child4: joining 3
SchedulerTest17-Child5: started
SchedulerTest17-Child5: joining 3
SchedulerTest17-Child6: started
SchedulerTest17-Child6: joining 3
SchedulerTest17-Child7: started
SchedulerTest17-Child7: joining 3
SchedulerTest17-Child8: started
SchedulerTest17-Child8: joining 3
SchedulerTest17-Child9: started
SchedulerTest17-Child9: joining 3
SchedulerTest17-Child10: started
SchedulerTest17-Child10: joining 3
SchedulerTest17-Child11: started
SchedulerTest17-Child11: joining 3
SchedulerTest17-Child1: started
SchedulerTest17-Child1: quitting
SchedulerTest17: exit status for child 3 is -3
SchedulerTest17: waiting for child process
SchedulerTest17-Child2: after joining child, pid 3, status = -3
SchedulerTest17: exit status for child 4 is 5
SchedulerTest17: waiting for child process
SchedulerTest17-Child3: after joining child, pid 3, status = -3
SchedulerTest17: exit status for child 5 is 5
SchedulerTest17: waiting for child process
SchedulerTest17-Child4: after joining child, pid 3, status = -3
SchedulerTest17: exit status for child 6 is 5
SchedulerTest17: waiting for child process
SchedulerTest17-Child5: after joining child, pid 3, status = -3
SchedulerTest17: exit status for child 7 is 5
SchedulerTest17: waiting for child process
SchedulerTest17-Child6: after joining child, pid 3, status = -3
SchedulerTest17: exit status for child 8 is 5
SchedulerTest17: waiting for child process
SchedulerTest17-Child7: after joining child, pid 3, status = -3
SchedulerTest17: exit status for child 9 is 5
SchedulerTest17: waiting for child process
SchedulerTest17-Child8: after joining child, pid 3, status = -3
SchedulerTest17: exit status for child 10 is 5
SchedulerTest17: waiting for child process
SchedulerTest17-Child9: after joining child, pid 3, status = -3
SchedulerTest17: exit status for child 11 is 5
SchedulerTest17: waiting for child process
SchedulerTest17-Child10: after joining child, pid 3, status = -3
SchedulerTest17: exit status for child 12 is 5
SchedulerTest17: waiting for child process
SchedulerTest17-Child11: after joining child, pid 3, status = -3
SchedulerTest17: exit status for child 13 is 5
All processes completed.

SchedulerTest18: started
spawn(): Stack size is too small
SchedulerTest18: after spawn of child with pid -2
SchedulerTest18: TEST PASSED
All processes completed.

SchedulerTest19: started
SchedulerTest19: waiting for child process
SchedulerTest19-Child1: started
SchedulerTest19-Child1: quitting
SchedulerTest19: exit status for child 3 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child2: started
SchedulerTest19-Child2: quitting
SchedulerTest19: exit status for child 4 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child3: started
SchedulerTest19-Child3: quitting
SchedulerTest19: exit status for child 5 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child4: started
SchedulerTest19-Child4: quitting
SchedulerTest19: exit status for child 6 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child5: started
SchedulerTest19-Child5: quitting
SchedulerTest19: exit status for child 7 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child6: started
SchedulerTest19-Child6: quitting
SchedulerTest19: exit status for child 8 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child7: started
SchedulerTest19-Child7: quitting
SchedulerTest19: exit status for child 9 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child8: started
SchedulerTest19-Child8: quitting
SchedulerTest19: exit status for child 10 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child9: started
SchedulerTest19-Child9: quitting
SchedulerTest19: exit status for child 11 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child10: started
SchedulerTest19-Child10: quitting
SchedulerTest19: exit status for child 12 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child11: started
SchedulerTest19-Child11: quitting
SchedulerTest19: exit status for child 13 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child12: started
SchedulerTest19-Child12: quitting
SchedulerTest19: exit status for child 14 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child13: started
SchedulerTest19-Child13: quitting
SchedulerTest19: exit status for child 15 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child14: started
SchedulerTest19-Child14: quitting
SchedulerTest19: exit status for child 16 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child15: started
SchedulerTest19-Child15: quitting
SchedulerTest19: exit status for child 17 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child16: started
SchedulerTest19-Child16: quitting
SchedulerTest19: exit status for child 18 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child17: started
SchedulerTest19-Child17: quitting
SchedulerTest19: exit status for child 19 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child18: started
SchedulerTest19-Child18: quitting
SchedulerTest19: exit status for child 20 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child19: started
SchedulerTest19-Child19: quitting
SchedulerTest19: exit status for child 21 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child20: started
SchedulerTest19-Child20: quitting
SchedulerTest19: exit status for child 22 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child21: started
SchedulerTest19-Child21: quitting
SchedulerTest19: exit status for child 23 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child22: started
SchedulerTest19-Child22: quitting
SchedulerTest19: exit status for child 24 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child23: started
SchedulerTest19-Child23: quitting
SchedulerTest19: exit status for child 25 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child24: started
SchedulerTest19-Child24: quitting
SchedulerTest19: exit status for child 26 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child25: started
SchedulerTest19-Child25: quitting
SchedulerTest19: exit status for child 27 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child26: started
SchedulerTest19-Child26: quitting
SchedulerTest19: exit status for child 28 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child27: started
SchedulerTest19-Child27: quitting
SchedulerTest19: exit status for child 29 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child28: started
SchedulerTest19-Child28: quitting
SchedulerTest19: exit status for child 30 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child29: started
SchedulerTest19-Child29: quitting
SchedulerTest19: exit status for child 31 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child30: started
SchedulerTest19-Child30: quitting
SchedulerTest19: exit status for child 32 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child31: started
SchedulerTest19-Child31: quitting
SchedulerTest19: exit status for child 33 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child32: started
SchedulerTest19-Child32: quitting
SchedulerTest19: exit status for child 34 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child33: started
SchedulerTest19-Child33: quitting
SchedulerTest19: exit status for child 35 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child34: started
SchedulerTest19-Child34: quitting
SchedulerTest19: exit status for child 36 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child35: started
SchedulerTest19-Child35: quitting
SchedulerTest19: exit status for child 37 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child36: started
SchedulerTest19-Child36: quitting
SchedulerTest19: exit status for child 38 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child37: started
SchedulerTest19-Child37: quitting
SchedulerTest19: exit status for child 39 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child38: started
SchedulerTest19-Child38: quitting
SchedulerTest19: exit status for child 40 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child39: started
SchedulerTest19-Child39: quitting
SchedulerTest19: exit status for child 41 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child40: started
SchedulerTest19-Child40: quitting
SchedulerTest19: exit status for child 42 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child41: started
SchedulerTest19-Child41: quitting
SchedulerTest19: exit status for child 43 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child42: started
SchedulerTest19-Child42: quitting
SchedulerTest19: exit status for child 44 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child43: started
SchedulerTest19-Child43: quitting
SchedulerTest19: exit status for child 45 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child44: started
SchedulerTest19-Child44: quitting
SchedulerTest19: exit status for child 46 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child45: started
SchedulerTest19-Child45: quitting
SchedulerTest19: exit status for child 47 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child46: started
SchedulerTest19-Child46: quitting
SchedulerTest19: exit status for child 48 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child47: started
SchedulerTest19-Child47: quitting
SchedulerTest19: exit status for child 49 is -3
SchedulerTest19: waiting for child process
SchedulerTest19-Child48: started
SchedulerTest19-Child48: quitting
SchedulerTest19: exit status for child 50 is -3
SchedulerTest19: waiting for child process
SchedulerTest19: exit status for child -1 is -3
SchedulerTest19: waiting for child process
SchedulerTest19: exit status for child -1 is -3
All processes completed.

SchedulerTest20: started
SchedulerTest20: after spawn of child with pid 3
SchedulerTest20: waiting for child process
SchedulerTest20-Child1: started
SchedulerTest20-Child1: joining with pid 3 (should fail)
join: process attempted to join itself.
THREADS Halted: Error code 1.

SchedulerTest21: started
spawn(): Priority out of range.
spawn(): Priority out of range.
SchedulerTest21: waiting for child process
SchedulerTest21: exit status for child -1 is -1
SchedulerTest21: waiting for child process
SchedulerTest21: exit status for child -1 is -1
All processes completed.

SchedulerTest22: started
SchedulerTest22: after spawn of child with pid 3
SchedulerTest22: waiting for child process
SchedulerTest22-Child1: started
SchedulerTest22-Child1: performing spawn of child
SchedulerTest22-Child1-Child1: started
SchedulerTest22-Child1: after spawn of child with pid 4
SchedulerTest22-Child1: performing spawn of child
SchedulerTest22-Child1-Child2: started
SchedulerTest22-Child1: after spawn of child with pid 5
SchedulerTest22-Child1: performing spawn of child
SchedulerTest22-Child1-Child3: started
SchedulerTest22-Child1: after spawn of child with pid 6
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    1        0        Scheduler
3       2        2         RUNNING       3        0        SchedulerTest22-Child1
4       3        3         14            0        0        SchedulerTest22-Child1-Child1
5       3        3         14            0        0        SchedulerTest22-Child1-Child2
6       3        3         14            0        0        SchedulerTest22-Child1-Child3
SchedulerTest22-Child1: Unblocking process 4
SchedulerTest22-Child1-Child1: quitting
SchedulerTest22-Child1: Unblocking process 5
SchedulerTest22-Child1-Child2: quitting
SchedulerTest22-Child1: Unblocking process 6
SchedulerTest22-Child1-Child3: quitting
SchedulerTest22-Child1: waiting for child process
SchedulerTest22-Child1: exit status for child 4 is -3
SchedulerTest22-Child1: waiting for child process
SchedulerTest22-Child1: exit status for child 5 is -3
SchedulerTest22-Child1: waiting for child process
SchedulerTest22-Child1: exit status for child 6 is -3
SchedulerTest22: exit status for child 3 is -2
All processes completed.

SchedulerTest23: started
SchedulerTest23: after spawn of child with pid 3
SchedulerTest23: joining child process
SchedulerTest23-Child1: started
SchedulerTest23-Child1: performing spawn of child
SchedulerTest23-Child1-Child1: started
SchedulerTest23-Child1: after spawn of child with pid 4
SchedulerTest23-Child1: performing spawn of child
SchedulerTest23-Child1-Child2: started
SchedulerTest23-Child1: after spawn of child with pid 5
SchedulerTest23-Child1: performing spawn of child
SchedulerTest23-Child1-Child3: started
SchedulerTest23-Child1: after spawn of child with pid 6
SchedulerTest23-Child1: performing spawn of child
SchedulerTest23-Child1-Child4: started
SchedulerTest23-Child1: after spawn of child with pid 7
SchedulerTest23-Child1: performing spawn of child
SchedulerTest23-Child1-Child5: started
SchedulerTest23-Child1: after spawn of child with pid 8
SchedulerTest23-Child1: performing spawn of child
SchedulerTest23-Child1-Child6: started
SchedulerTest23-Child1: after spawn of child with pid 9
SchedulerTest23-Child1: performing spawn of child
SchedulerTest23-Child1-Child7: started
SchedulerTest23-Child1: after spawn of child with pid 10
SchedulerTest23-Child1: performing spawn of child
SchedulerTest23-Child1-Child8: started
SchedulerTest23-Child1: after spawn of child with pid 11
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    1        0        Scheduler
3       2        2         RUNNING       8        0        SchedulerTest23-Child1
4       3        3         14            0        0        SchedulerTest23-Child1-Child1
5       3        3         14            0        0        SchedulerTest23-Child1-Child2
6       3        3         14            0        0        SchedulerTest23-Child1-Child3
7       3        3         14            0        0        SchedulerTest23-Child1-Child4
8       3        4         14            0        0        SchedulerTest23-Child1-Child5
9       3        4         14            0        0        SchedulerTest23-Child1-Child6
10      3        4         14            0        0        SchedulerTest23-Child1-Child7
11      3        4         14            0        0        SchedulerTest23-Child1-Child8
SchedulerTest23-Child1: performing spawn of child
SchedulerTest23-Child1-Child8: started
SchedulerTest23-Child1-Child8: Unblocking process 4
SchedulerTest23-Child1-Child8: Unblocking process 5
SchedulerTest23-Child1-Child8: Unblocking process 6
SchedulerTest23-Child1-Child8: Unblocking process 7
SchedulerTest23-Child1-Child8: Unblocking process 8
SchedulerTest23-Child1-Child8: Unblocking process 9
SchedulerTest23-Child1-Child8: Unblocking process 10
SchedulerTest23-Child1-Child8: Unblocking process 11
SchedulerTest23-Child1-Child5: quitting
SchedulerTest23-Child1-Child6: quitting
SchedulerTest23-Child1-Child7: quitting
SchedulerTest23-Child1-Child8: quitting
SchedulerTest23-Child1-Child1: quitting
SchedulerTest23-Child1-Child2: quitting
SchedulerTest23-Child1-Child3: quitting
SchedulerTest23-Child1-Child4: quitting
SchedulerTest23-Child1: after spawn of child with pid 12
SchedulerTest23-Child1: waiting for child process
SchedulerTest23-Child1: exit status for child 12 is -2
SchedulerTest23-Child1: waiting for child process
SchedulerTest23-Child1: exit status for child 8 is -3
SchedulerTest23-Child1: waiting for child process
SchedulerTest23-Child1: exit status for child 9 is -3
SchedulerTest23-Child1: waiting for child process
SchedulerTest23-Child1: exit status for child 10 is -3
SchedulerTest23-Child1: waiting for child process
SchedulerTest23-Child1: exit status for child 11 is -3
SchedulerTest23-Child1: waiting for child process
SchedulerTest23-Child1: exit status for child 4 is -3
SchedulerTest23-Child1: waiting for child process
SchedulerTest23-Child1: exit status for child 5 is -3
SchedulerTest23-Child1: waiting for child process
SchedulerTest23-Child1: exit status for child 6 is -3
SchedulerTest23-Child1: waiting for child process
SchedulerTest23-Child1: exit status for child 7 is -3
SchedulerTest23: exit status for child 3 is -2
All processes completed.

SchedulerTest24: started
SchedulerTest24: after spawn of child with pid 3
SchedulerTest24: joining child process
SchedulerTest24-Child1: started
SchedulerTest24-Child1: performing spawn of child
SchedulerTest24-Child1-Child1: started
SchedulerTest24-Child1: after spawn of child with pid 4
SchedulerTest24-Child1: performing spawn of child
SchedulerTest24-Child1-Child2: started
SchedulerTest24-Child1: after spawn of child with pid 5
SchedulerTest24-Child1: performing spawn of child
SchedulerTest24-Child1-Child3: started
SchedulerTest24-Child1: after spawn of child with pid 6
SchedulerTest24-Child1: performing spawn of child
SchedulerTest24-Child1-Child4: started
SchedulerTest24-Child1: after spawn of child with pid 7
SchedulerTest24-Child1: performing spawn of child
SchedulerTest24-Child1-Child5: started
SchedulerTest24-Child1: after spawn of child with pid 8
SchedulerTest24-Child1: performing spawn of child
SchedulerTest24-Child1-Child6: started
SchedulerTest24-Child1: after spawn of child with pid 9
SchedulerTest24-Child1: performing spawn of child
SchedulerTest24-Child1-Child7: started
SchedulerTest24-Child1: after spawn of child with pid 10
SchedulerTest24-Child1: performing spawn of child
SchedulerTest24-Child1-Child8: started
SchedulerTest24-Child1: after spawn of child with pid 11
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    1        0        Scheduler
3       2        2         RUNNING       8        0        SchedulerTest24-Child1
4       3        4         14            0        0        SchedulerTest24-Child1-Child1
5       3        4         14            0        0        SchedulerTest24-Child1-Child2
6       3        4         14            0        0        SchedulerTest24-Child1-Child3
7       3        4         14            0        0        SchedulerTest24-Child1-Child4
8       3        3         14            0        0        SchedulerTest24-Child1-Child5
9       3        3         14            0        0        SchedulerTest24-Child1-Child6
10      3        3         14            0        0        SchedulerTest24-Child1-Child7
11      3        3         14            0        0        SchedulerTest24-Child1-Child8
SchedulerTest24-Child1: performing spawn of child
SchedulerTest24-Child1-Child8: started
SchedulerTest24-Child1-Child8: Unblocking process 4
SchedulerTest24-Child1-Child8: Unblocking process 5
SchedulerTest24-Child1-Child8: Unblocking process 6
SchedulerTest24-Child1-Child8: Unblocking process 7
SchedulerTest24-Child1-Child8: Unblocking process 8
SchedulerTest24-Child1-Child8: Unblocking process 9
SchedulerTest24-Child1-Child8: Unblocking process 10
SchedulerTest24-Child1-Child8: Unblocking process 11
SchedulerTest24-Child1-Child1: quitting
SchedulerTest24-Child1-Child2: quitting
SchedulerTest24-Child1-Child3: quitting
SchedulerTest24-Child1-Child4: quitting
SchedulerTest24-Child1-Child5: quitting
SchedulerTest24-Child1-Child6: quitting
SchedulerTest24-Child1-Child7: quitting
SchedulerTest24-Child1-Child8: quitting
SchedulerTest24-Child1: after spawn of child with pid 12
SchedulerTest24-Child1: waiting for child process
SchedulerTest24-Child1: exit status for child 12 is -2
SchedulerTest24-Child1: waiting for child process
SchedulerTest24-Child1: exit status for child 4 is -3
SchedulerTest24-Child1: waiting for child process
SchedulerTest24-Child1: exit status for child 5 is -3
SchedulerTest24-Child1: waiting for child process
SchedulerTest24-Child1: exit status for child 6 is -3
SchedulerTest24-Child1: waiting for child process
SchedulerTest24-Child1: exit status for child 7 is -3
SchedulerTest24-Child1: waiting for child process
SchedulerTest24-Child1: exit status for child 8 is -3
SchedulerTest24-Child1: waiting for child process
SchedulerTest24-Child1: exit status for child 9 is -3
SchedulerTest24-Child1: waiting for child process
SchedulerTest24-Child1: exit status for child 10 is -3
SchedulerTest24-Child1: waiting for child process
SchedulerTest24-Child1: exit status for child 11 is -3
SchedulerTest24: exit status for child 3 is -2
All processes completed.

SchedulerTest25: started
SchedulerTest25: after spawn of child with pid 3
SchedulerTest25: joining child process
P1-Child1: started
P1-Child1: quitting
SchedulerTest25: exit status for child 3 is -3
All processes completed.

SchedulerTest26: started
SchedulerTest26: after spawn of child with pid 3
SchedulerTest26: after spawn of child with pid 4
SchedulerTest26: waiting for child process
SchedulerTest26-Child1: started
SchedulerTest26-Child1: quitting
SchedulerTest26: exit status for child 3 is -3
SchedulerTest26: waiting for child process
SchedulerTest26-Child2: started
SchedulerTest26-Child2: joining target
join: attempting to join a process that does not exist.
THREADS Halted: Error code 1.

SchedulerTest27: started
SchedulerTest27: after spawn of child with pid 3
SchedulerTest27: after spawn of child with pid 4
SchedulerTest27: after spawn of child with pid 5
SchedulerTest27: waiting for child process
SchedulerTest27-Child1: started
SchedulerTest27-Child1: performing spawn of child
SchedulerTest27-Child1-Child1: started
SchedulerTest27-Child1-Child1: quitting
SchedulerTest27-Child2: started
SchedulerTest27-Child2: joining target
SchedulerTest27-Child3: started
SchedulerTest27-Child3: performing spawn of first child
SchedulerTest27-Child3-Child1: started
SchedulerTest27-Child3-Child1: quitting
SchedulerTest27-Child1: spawn of child returned pid = 6
SchedulerTest27-Child1: exit status for child 6 is -3
SchedulerTest27: exit status for child 3 is -3
SchedulerTest27: waiting for child process
SchedulerTest27-Child3: spawn of first child returned pid = 7
SchedulerTest27-Child3: performing spawn of second child
SchedulerTest27-Child3-Child2: started
SchedulerTest27-Child3-Child2: quitting
SchedulerTest27-Child3: spawn of second child returned pid = 8
SchedulerTest27-Child3: performing first wait
SchedulerTest27-Child3: exit status for child 7 is -3
SchedulerTest27-Child3: performing second wait
SchedulerTest27-Child3: exit status for child 8 is -3
SchedulerTest27: exit status for child 5 is -3
SchedulerTest27: waiting for child process
SchedulerTest27-Child2: k_join for pid 5 returned 0 with status -3
SchedulerTest27: exit status for child 4 is -2
All processes completed.

SchedulerTest28: started
SchedulerTest28: after spawn of child with pid 3
SchedulerTest28: after spawn of child with pid 4
SchedulerTest28: after spawn of child with pid 5
SchedulerTest28: waiting for child process
SchedulerTest28-Child1: started
SchedulerTest28-Child1: quitting
SchedulerTest28: exit status for child 3 is -3
SchedulerTest28: waiting for child process
SchedulerTest28-Child2: started
SchedulerTest28-Child2: joining target
SchedulerTest28-Child3: started
SchedulerTest28-Child3: performing spawn of child
SchedulerTest28-Child3: spawn of child returned pid = 6
SchedulerTest28-Child3-Child1: started
SchedulerTest28-Child3-Child1: quitting
SchedulerTest28-Child3-Child1: exit status for child 6 is -3
SchedulerTest28: exit status for child 5 is -3
SchedulerTest28: waiting for child process
SchedulerTest28-Child2: k_join for pid 5 returned 0 with status -3
SchedulerTest28: exit status for child 4 is -2
All processes completed.

SchedulerTest29: started
SchedulerTest29: after spawn of child with pid 3
SchedulerTest29: after spawn of child with pid 4
SchedulerTest29: after spawn of child with pid 5
SchedulerTest29: after spawn of child with pid 6
SchedulerTest29: after spawn of child with pid 7
SchedulerTest29-Child1: started
SchedulerTest29-Child1: performing spawn of first child
SchedulerTest29-Child2: started
SchedulerTest29-Child2: started, child number is 2
SchedulerTest29-Child3: started
SchedulerTest29-Child3: started, child number is 3
SchedulerTest29-Child4: started
SchedulerTest29-Child4: started, child number is 4
SchedulerTest29-Child5: started
SchedulerTest29-Child5: started, child number is 5
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    5        0        Scheduler
3       2        3         READY         0        633      SchedulerTest29-Child1
4       2        3         READY         0        637      SchedulerTest29-Child2
5       2        3         READY         0        638      SchedulerTest29-Child3
6       2        3         RUNNING       0        543      SchedulerTest29-Child4
7       2        3         READY         0        547      SchedulerTest29-Child5
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    5        0        Scheduler
3       2        3         READY         0        1542     SchedulerTest29-Child1
4       2        3         READY         0        1528     SchedulerTest29-Child2
5       2        3         READY         0        1544     SchedulerTest29-Child3
6       2        3         RUNNING       0        1479     SchedulerTest29-Child4
7       2        3         READY         0        1457     SchedulerTest29-Child5
SchedulerTest29-Child2: exiting, pid = 4
SchedulerTest29: exit status for child 4 is -4
SchedulerTest29-Child3: exiting, pid = 5
SchedulerTest29: exit status for child 5 is -5
SchedulerTest29-Child4: exiting, pid = 6
SchedulerTest29: exit status for child 6 is -6
SchedulerTest29-Child5: exiting, pid = 7
SchedulerTest29: exit status for child 7 is -7
SchedulerTest29: after spawn of child with pid 8
SchedulerTest29: after spawn of child with pid 9
SchedulerTest29: after spawn of child with pid 10
SchedulerTest29: after spawn of child with pid 11
SchedulerTest29-Child6: started
SchedulerTest29-Child6: started, child number is 6
SchedulerTest29-Child7: started
SchedulerTest29-Child7: started, child number is 7
SchedulerTest29-Child8: started
SchedulerTest29-Child8: started, child number is 8
SchedulerTest29-Child9: started
SchedulerTest29-Child9: started, child number is 9
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    5        0        Scheduler
3       2        3         READY         0        2727     SchedulerTest29-Child1
8       2        3         READY         0        641      SchedulerTest29-Child6
9       2        3         READY         0        637      SchedulerTest29-Child7
10      2        3         RUNNING       0        548      SchedulerTest29-Child8
11      2        3         READY         0        543      SchedulerTest29-Child9
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    5        0        Scheduler
3       2        3         READY         0        3634     SchedulerTest29-Child1
8       2        3         READY         0        1546     SchedulerTest29-Child6
9       2        3         READY         0        1546     SchedulerTest29-Child7
10      2        3         RUNNING       0        1489     SchedulerTest29-Child8
11      2        3         READY         0        1456     SchedulerTest29-Child9
SchedulerTest29-Child6: exiting, pid = 8
SchedulerTest29: exit status for child 8 is -8
SchedulerTest29-Child7: exiting, pid = 9
SchedulerTest29: exit status for child 9 is -9
SchedulerTest29-Child8: exiting, pid = 10
SchedulerTest29: exit status for child 10 is -10
SchedulerTest29-Child9: exiting, pid = 11
SchedulerTest29: exit status for child 11 is -11
SchedulerTest29: after spawn of child with pid 12
SchedulerTest29: after spawn of child with pid 13
SchedulerTest29: after spawn of child with pid 14
SchedulerTest29: after spawn of child with pid 15
SchedulerTest29-Child10: started
SchedulerTest29-Child10: started, child number is 10
SchedulerTest29-Child11: started
SchedulerTest29-Child11: started, child number is 11
SchedulerTest29-Child12: started
SchedulerTest29-Child12: started, child number is 12
SchedulerTest29-Child13: started
SchedulerTest29-Child13: started, child number is 13
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    5        0        Scheduler
3       2        3         READY         0        4805     SchedulerTest29-Child1
12      2        3         READY         0        637      SchedulerTest29-Child10
13      2        3         READY         0        636      SchedulerTest29-Child11
14      2        3         RUNNING       0        547      SchedulerTest29-Child12
15      2        3         READY         0        547      SchedulerTest29-Child13
PID     Parent   Priority  Status        # Kids   CPUtime  Name    
1       -1       0         READY         0        0        watchdog
2       -1       5         WAIT BLOCK    5        0        Scheduler
3       2        3         READY         0        5720     SchedulerTest29-Child1
12      2        3         READY         0        1540     SchedulerTest29-Child10
13      2        3         READY         0        1545     SchedulerTest29-Child11
14      2        3         RUNNING       0        1508     SchedulerTest29-Child12
15      2        3         READY         0        1457     SchedulerTest29-Child13
SchedulerTest29-Child10: exiting, pid = 12
SchedulerTest29: exit status for child 12 is -12
SchedulerTest29-Child11: exiting, pid = 13
SchedulerTest29: exit status for child 13 is -13
SchedulerTest29-Child12: exiting, pid = 14
SchedulerTest29: exit status for child 14 is -14
SchedulerTest29-Child13: exiting, pid = 15
SchedulerTest29: exit status for child 15 is -15
SchedulerTest29: exit status for child 3 is -5
All processes completed.

SchedulerTest30: started
SchedulerTest30: performing spawn of child
SchedulerTest30: after spawn of child with pid 3
SchedulerTest30: performing spawn of child
SchedulerTest30: after spawn of child with pid 4
SchedulerTest30: performing spawn of child
SchedulerTest30: after spawn of child with pid 5
SchedulerTest30: waiting for child process
SchedulerTest30-Child1: started
SchedulerTest30-Child2: started
SchedulerTest30-Child3: started
SchedulerTest30-Child1: calling readTime
SchedulerTest30-Child1: readTime returned 361
SchedulerTest30-Child1: quitting
SchedulerTest30: exit status for child 3 is -3
SchedulerTest30: waiting for child process
SchedulerTest30-Child2: calling readTime
SchedulerTest30-Child2: readTime returned 849
SchedulerTest30-Child2: quitting
SchedulerTest30: exit status for child 4 is -3
SchedulerTest30: waiting for child process
SchedulerTest30-Child3: calling readTime
SchedulerTest30-Child3: readTime returned 1873
SchedulerTest30-Child3: quitting
SchedulerTest30: exit status for child 5 is -3
All processes completed.

SchedulerTest31: started
SchedulerTest31: blocking with a value of 6
block: function called with a reserved status value.
THREADS Halted: Error code 1.


```
