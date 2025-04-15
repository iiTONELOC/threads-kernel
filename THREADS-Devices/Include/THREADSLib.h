#pragma once
#include <Windows.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef THREADS_BUILD
#define LIB_SPEC __declspec(dllexport) 
#else
#define LIB_SPEC
#endif

/* System wide definitions and constants. */
#define THREADS_MIN_STACK_SIZE		    8192
#define THREADS_MAX_NAME				128
#define THREADS_MAX_SYSCALLS			32

#define THREADS_MAX_DISKS               2
#define THREADS_MAX_TERMINALS			4
#define THREADS_CLOCK_DEVICE_ID         0
#define THREADS_SYSTEM_CALL_ID          7
#define THREADS_MAX_DEVICES             8

#define THREADS_MAX_IO_BUFFER_SIZE		1024
#define THREADS_MAX_DEVICE_NAME			32

#define MAX_PROCESSES					50

/* Interrupt identifiers.  Indicies into the interrupt vector. */
#define THREADS_TIMER_INTERRUPT			0
#define THREADS_IO_INTERRUPT			1
#define THREADS_EXCEPTION_INTERRUPT		2
#define THREADS_SYS_CALL_INTERRUPT		3
#define THREADS_INTERRUPT_HANDLER_COUNT 4

#define PSR_INTERRUPTS          1
#define PSR_KERNEL_MODE         2
#define PSR_IRQ_MODE            4

/* Device commands. */
#define DISK_INFO             0x01  
#define DISK_READ             0x04
#define DISK_WRITE            0x08
#define DISK_SEEK			  0x10
#define TERMINAL_READ_CHAR    0x20
#define TERMINAL_WRITE_CHAR   0x40
#define SYSTEM_CALL           0x80  

/* Device types */
typedef enum
{
	DEVICE_TERMINAL = 0,
	DEVICE_CLOCK,
	DEVICE_DISK
} device_type_t;

/* structure for device control and i/o*/
typedef struct
{
	uint8_t		  command;      /* command to invoke */
	uint8_t		  control1;     /* device specific value */
	uint8_t		  control2;     /* device specific value */
	void*		  input_data;   /* incoming data */
	void*		  output_data;  /* outgoing data */
	uint32_t	  data_length;  /* data length */
} device_control_block_t;

#define THREADS_DISK_SECTOR_SIZE    512
#define THREADS_DISK_SECTOR_COUNT   16   /* Sectors per track*/
#define THREADS_DISK_MAX_PLATTERS   3
#define THREADS_DISK_MAX_TRACKS     256   /* Max number of tracks */

/* structure passed to system calls. */
#define THREADS_MAX_SYSCALL_ARGUMENTS  6
typedef struct
{
	uint32_t  call_id;
	intptr_t  arguments[THREADS_MAX_SYSCALL_ARGUMENTS];
} system_call_arguments_t;

typedef int (*process_entrypoint_t) (void*);
typedef void (*interrupt_handler_t) (char deviceId[32], uint8_t command, uint32_t status, void *pArgs);   /* function where process begins */


/* THREADS Interface */

LIB_SPEC void*		 context_initialize(process_entrypoint_t entry_point, int stack_size, void* args);
LIB_SPEC bool        context_switch(LPVOID next_context);
LIB_SPEC void	     context_stop(LPVOID context);

LIB_SPEC uint32_t    get_psr();
LIB_SPEC void	     set_psr(uint32_t psr);
LIB_SPEC uint32_t    system_clock();

LIB_SPEC interrupt_handler_t*	get_interrupt_handlers();

LIB_SPEC uint32_t    device_initialize(char* device);
LIB_SPEC uint32_t    device_handle(char* device);
LIB_SPEC uint32_t    device_control(char* device, device_control_block_t control_block);

LIB_SPEC void        set_debug_level(int level);
LIB_SPEC void	     console_output(bool debug, char* string, ...);

LIB_SPEC void		 stop(int code);

LIB_SPEC void system_call(system_call_arguments_t* sys_args);

