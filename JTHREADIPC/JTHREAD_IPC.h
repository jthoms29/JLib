#ifndef JTHREAD_IPC_H
#define JTHREAD_IPC_H
#include <stddef.h>
#include <stdio.h>
#include <../JLIST/JLIST.h>
#include <pthread.h>

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

#endif