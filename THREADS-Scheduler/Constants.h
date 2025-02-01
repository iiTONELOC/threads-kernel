#pragma once
#ifndef SCHEDULER_CONSTANTS_H
#define SCHEDULER_CONSTANTS_H

#define LOWEST_PRIORITY 0  // Lowest priority level - where watchdog runs
#define HIGHEST_PRIORITY 5 // Highest priority level - where the scheduler runs

/*Priority Levels 0-5*/
// TODO: Rename these to be more descriptive
#define PRIORITY_LEVEL_0 0
#define PRIORITY_LEVEL_1 1
#define PRIORITY_LEVEL_2 2
#define PRIORITY_LEVEL_3 3
#define PRIORITY_LEVEL_4 4
#define PRIORITY_LEVEL_5 5

#define MAXNAME 256
#define MAXARG 256
#define MAXPROC 50

/* Kill signals */
#define SIG_TERM 15

/* Process status */
#define NUM_PROCESS_STATES 4
#define STATUS_READY 0
#define STATUS_RUNNING 1
#define STATUS_BLOCKED_WAIT 2
#define STATUS_QUIT 3

/* Max Processes*/
#ifndef MAX_PROCESSES
#define MAX_PROCESSES 50
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

/* Time Slice */
#define MIN_TIME_SLICE_MS 20
#define MAX_TIME_SLICE_MS 50
#define DEFAULT_TIME_SLICE_MS 25
#define NUM_MILLI_SEC_IN_MICRO_SEC 1000

#endif
