#ifndef JTHREAD_IPC_H
#define JTHREAD_IPC_H
#include <stddef.h>
#include <stdio.h>
#include <pthread.h>
#include <../JLIST/JLIST.h>
#include <../JHASHMAP/JHASHMAP.h>

typedef struct JMESSAGE {
    void* message;
    size_t message_size;
    pthread_t sender;
} JMESSAGE;

typedef struct JMESSAGE_QUEUE {
    /* Queue of messages that can be sent to this thread */
    JLIST* list;

    /* Mutex and condition variable. The thread will block after sending a message*/

} JMESSAGE_QUEUE;


typedef JHASHMAP JMESSAGE_QUEUE_DIR;

/*
 * Sends a message to the thread of this index in the thread table. Blocks, so
 * will only be awakened when a message is sent back.
 */
int JTHREAD_IPC_send(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t* thread, void* message);

/*
 * Sends a message and wakes the specified thread.. Does not block, so
 * the sending thread remains active. 
 */
int JTHREAD_IPC_reply(JMESSAGE* message);



/* ##########################################################################
    The following functions are for the message queue directory. Use this to get the message queue associated
    with a specific thread id.
*/

/*

*/
JMESSAGE_QUEUE_DIR* JMESSAGE_QUEUE_DIR_new(void);

/*

*/
int JMESSAGE_QUEUE_DIR_new_queue(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t* thread);

JMESSAGE_QUEUE* JMESSAGE_QUEUE_DIR_get_queue(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t* thread);

#endif

