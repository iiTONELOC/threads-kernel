#pragma once
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
#define READY 0
#define RUNNING 1
#define BLOCKED 2

#ifndef NULL
#define NULL ((void *)0)
#endif