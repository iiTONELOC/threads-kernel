#pragma once


/*
 * Maximum line length
 */

#define MAXLINE         80

 /*
  * Function prototypes for this phase.
  */

extern  int  sys_sleep(int seconds);
int sys_disk_read(char* deviceName, void* dataBuffer, int platter, int track, int firstSector, int sectors, int* status);
extern  int  sys_disk_write(int unit, int track, int first, int sectors, void* buffer);
int  sys_disk_info(int unit, int* platters, int* sectors, int* tracks, int* disk);
extern  int  sys_term_read(int unit, int size, char* buffer);
extern  int  sys_term_write(int unit, int size, char* text);


#define ERR_INVALID             -1
#define ERR_OK                  0


#pragma once
