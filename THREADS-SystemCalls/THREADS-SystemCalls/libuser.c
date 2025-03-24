/*
 *  File:  libuser.c
 *
 *  Description:  This file contains the interface declarations
 *                to the OS kernel support package.
 *
 */

//#include <libuser.h>
#include <THREADSLib.h>
#include <SystemCalls.h>


#define CHECKMODE {						\
	if (get_psr() & PSR_KERNEL_MODE) { 				\
	    console_output(FALSE, "Trying to invoke system_call from kernel\n");	\
	    stop(1);						\
	}							\
}

/*
 *  Routine:  Spawn
 *
 *  Description: This is the call entry to fork a new user process.
 *
 *  Arguments:    char *name    -- new process's name
 *		  PFV func      -- pointer to the function to fork
 *		  void *arg	-- argument to function
 *                int stacksize -- amount of stack to be allocated
 *                int priority  -- priority of forked process
 *                int  *pid      -- pointer to output value
 *                (output value: process id of the forked process)
 *
 *  Return Value: 0 means success, -1 means error occurs
 *
 */
int Spawn(char *name, int (*func)(char *), char *arg, int stack_size,
	int priority, int *pid)   
{
    system_call_arguments_t sa;
    
    CHECKMODE;
    sa.call_id = SYS_SPAWN;
    sa.arguments[0] = (intptr_t) func;
    sa.arguments[1] = (intptr_t) arg;
    sa.arguments[2] = (intptr_t) stack_size;
    sa.arguments[3] = (intptr_t) priority;
    sa.arguments[4] = (intptr_t) name;
    system_call(&sa);
    *pid = (int) sa.arguments[0];
    return (int) sa.arguments[3];
} /* end of Spawn */


/*
 *  Routine:  Wait
 *
 *  Description: This is the call entry to wait for a child completion
 *
 *  Arguments:    int *pid -- pointer to output value 1
 *                (output value 1: process id of the completing child)
 *                int *status -- pointer to output value 2
 *                (output value 2: status of the completing child)
 *
 *  Return Value: 0 means success, -1 means error occurs
 *
 */
int Wait(int *pid, int *status)	
{
    system_call_arguments_t sa;
    
    CHECKMODE;
    sa.call_id = SYS_WAIT;
    system_call(&sa);
    *pid = (int) sa.arguments[0];
    *status = (int) sa.arguments[1];
    return (int) sa.arguments[3];
    
} /* End of Wait */


/*
 *  Routine:  Exit
 *
 *  Description: This is the call entry to Exit 
 *               the invoking process and its children
 *
 *  Arguments:   int status -- the commpletion status of the process
 *
 *  Return Value: 0 means success, -1 means error occurs
 *
 */
void Exit(int status)
{
    system_call_arguments_t sa;
    
    CHECKMODE;
    sa.call_id = SYS_EXIT;
    sa.arguments[0] = (intptr_t) status;
    system_call(&sa);
    return;
    
} /* End of Exit */


/*
 *  Routine:  SemCreate
 *
 *  Description: Create a semaphore.
 *		
 *
 *  Arguments:      int value -- initial semaphore value
 *		            int *semaphore -- semaphore handle
 *                      (output value: completion status)
 *
 */
int SemCreate(int value, int *semaphore)
{
    system_call_arguments_t sa;

    CHECKMODE;
    sa.call_id = SYS_SEMCREATE;
    sa.arguments[0] = (intptr_t) value;
    system_call(&sa);
    *semaphore = (int) sa.arguments[0];
    return (int) sa.arguments[3];
} /* end of SemCreate */


/*
 *  Routine:  SemP
 *
 *  Description: "P" a semaphore.
 *		
 *
 *  Arguments:    int semaphore -- semaphore handle
 *                (output value: completion status)
 *
 */
int SemP(int semaphore)
{
    system_call_arguments_t sa;

    CHECKMODE;
    sa.call_id = SYS_SEMP;
    sa.arguments[0] = (intptr_t) semaphore;
    system_call(&sa);
    return (int) sa.arguments[3];
} /* end of SemP */


/*
 *  Routine:  SemV
 *
 *  Description: "V" a semaphore.
 *		
 *
 *  Arguments:    int semaphore -- semaphore handle
 *                (output value: completion status)
 *
 */
int SemV(int semaphore)
{
    system_call_arguments_t sa;

    CHECKMODE;
    sa.call_id = SYS_SEMV;
    sa.arguments[0] = (intptr_t) semaphore;
    system_call(&sa);
    return (int) sa.arguments[3];
} /* end of SemV */


/*
 *  Routine:  SemFree
 *
 *  Description: Free a semaphore.
 *		
 *
 *  Arguments:    int semaphore -- semaphore handle
 *                (output value: completion status)
 *
 */
int SemFree(int semaphore)
{
    system_call_arguments_t sa;

    CHECKMODE;
    sa.call_id = SYS_SEMFREE;
    sa.arguments[0] = (intptr_t) semaphore;
    system_call(&sa);
    return (int) sa.arguments[3];
} /* end of SemFree */


/*
 *  Routine:  GetTimeofDay
 *
 *  Description: This is the call entry point for getting the time of day.
 *
 *  Arguments:    int *tod  -- pointer to output value
 *                (output value: the time of day)
 *
 */
void GetTimeofDay(int *tod)                           
{
    system_call_arguments_t sa;
    
    CHECKMODE;
    sa.call_id = SYS_GETTIMEOFDAY;
    system_call(&sa);
    *tod = (int) sa.arguments[0];
    return;
} /* end of GetTimeofDay */


/*
 *  Routine:  CPUTime
 *
 *  Description: This is the call entry point for the process' CPU time.
 *		
 *
 *  Arguments:    int *cpu  -- pointer to output value
 *                (output value: the CPU time of the process)
 *
 */
void CPUTime(int *cpu)                           
{
    system_call_arguments_t sa;

    CHECKMODE;
    sa.call_id = SYS_CPUTIME;
    system_call(&sa);
    *cpu = (int) sa.arguments[0];
    return;
} /* end of CPUTime */


/*
 *  Routine:  GetPID
 *
 *  Description: This is the call entry point for the process' PID.
 *		
 *
 *  Arguments:    int *pid  -- pointer to output value
 *                (output value: the PID)
 *
 */
void GetPID(int *pid)                           
{
    system_call_arguments_t sa;

    CHECKMODE;
    sa.call_id = SYS_GETPID;
    system_call(&sa);
    *pid = (int) sa.arguments[0];
    return;
} /* end of GetPID */

/* end libuser.c */
