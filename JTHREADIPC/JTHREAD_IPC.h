#ifndef JTHREAD_IPC_H
#define JTHREAD_IPC_H
#include <stddef.h>
#include <stdio.h>
#include <../JLIST/JLIST.h>
#include <pthread.h>
#include <./support/JHASHMAP.h>

typedef struct JMESSAGE {
    void* message;
    size_t message_size;
    size_t sender;
} JMESSAGE;

typedef struct JMESSAGE_QUEUE {
    /* Queue of messages that can be sent to this thread */
    JLIST* queue;

    /* Mutex and condition variable. The thread will block after sending a message*/
    pthread_mutex_t mutex;
    pthread_cond_t wait;

} JMESSAGE_QUEUE;


typedef JHASHMAP JMESSAGE_QUEUE_DIR;

/*
 * Sends a message to the thread of this index in the thread table. Blocks, so
 * will only be awakened when a message is sent back.
 */
int JTHREAD_IPC_send_block(JMESSAGE_QUEUE* thread_queue, void* message);

/*
 * Sends a message to the thread of this index in the thread table. Does not block, so
 * the sending thread remains active.
 */
int JTHREAD_IPC_send(size_t thread_idx, JMESSAGE* message);



/* ##########################################################################
    The following functions are for the message queue directory. Use this to get the message queue associated
    with a specific thread id.
*/

/*

*/
JMESSAGE_QUEUE_DIR* JMESSAGE_QUEUE_DIR_new(void);

/*

*/
int JMESSAGE_QUEUE_DIR_new_queue(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t thread_id);

JMESSAGE_QUEUE* JMESSAGE_QUEUE_DIR_get_queue(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t thread_id);

#endif