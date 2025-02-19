#pragma once

 /*
  * Maximum line length. Used by terminal read and write.
  */
#define MAXLINE         80

#define MAXMBOX         2000 /* 500 */
#define MAXSLOTS        2500 /* 5000 */
#define MAX_MESSAGE     256  /* largest possible message in a single slot */

/* returns id of mailbox, or -1 if no more mailboxes, -2 if invalid args */
int mailbox_create(int slots, int slot_size);

typedef struct mqattr {
    long mq_flags;       /* Flags (ignored for mq_open()) */
    long mq_maxmsg;      /* Max. # of messages on queue */
    long mq_msgsize;     /* Max. message size (bytes) */
    long mq_curmsgs;     /* # of messages currently in queue
                            (ignored for mq_open()) */
} MqAttributes;

#define MQ_O_CREAT     /* Create the message queue if it does not exist.*/
#define MQ_O_NONBLOCK  /* Open the queue in nonblocking mode.In circumstances
                          where mq_receive(3) and mq_send(3) would normally block,
                          these functions instead fail with the error EAGAIN.
                          int mqOpen(char *name, int oflags, MqAttributes *pAttrs);
                       */

/* returns 0 if successful, -1 if invalid arg */
extern int mailbox_free(int mbox_id);

/* returns 0 if successful, -1 if invalid args */
extern int mailbox_send(int mbox_id, void* msg_ptr, int msg_size, BOOL block);

/* returns size of received msg if successful, -1 if invalid args */
extern int mailbox_receive(int mbox_id, void* msg_ptr, int msg_max_size, BOOL block);

/* type = interrupt device type, unit = # of device (when more than one),
 * status = where interrupt handler puts device's status register.
 */
extern int wait_device(char* deviceName, int* status);

/*  The sysargs structure */
typedef struct sysargs
{
    int number;
    void* arg1;
    void* arg2;
    void* arg3;
    void* arg4;
    void* arg5;
} sysargs;

extern void  (*systemCallVector[])(system_call_arguments_t* args);

