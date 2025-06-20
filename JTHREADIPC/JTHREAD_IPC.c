#include <JTHREAD_IPC.h>

/*
int JTHREAD_IPC_send_block(JMESSAGE_QUEUE* receiver_queue, void* message) {
    if (receiver_queue == NULL) {
        printf("Error: thread's message queue is null.\n");
        return 1;
    }

    //acquire mutex before accessing thread's message queue
    pthread_mutex_lock(&receiver_queue->mutex);
        JLIST_append(receiver_queue->queue, message);
        //signal this thread, if it is sleeping
        pthread_condition_signal(receiver_queue->wait);    
    pthread_mutex_unlock(&receiver_queue->mutex);

    //then wait on own condition variable.
    return 0;
}

int JTHREAD_IPC_send(JMESSAGE_QUEUE* receiver_queue, void* message) {
    if (receiver_queue == NULL) {
        printf("Error: thread's message queue is null.\n");
    }

}
*/

pthread_t JMESSAGE_QUEUE_DIR_hash_pthread_t(void* key, size_t map_capacity) {
    return *((pthread_t*)key) % map_capacity;
}

bool JMESSAGE_QUEUE_DIR_compare_pthread_t(void* key1, void* key2) {
    pthread_t k1 = *((pthread_t*)key1);
    pthread_t k2 = *((pthread_t*)key2);

    return k1 == k2;
}



JMESSAGE_QUEUE_DIR* JMESSAGE_QUEUE_DIR_new(void) {
    /* Create a new hashmap. */
    JMESSAGE_QUEUE_DIR* queue_dir = JHASHMAP_new(
        JMESSAGE_QUEUE_DIR_hash_pthread_t, 
        JMESSAGE_QUEUE_DIR_compare_pthread_t );

    return queue_dir;
}


int JMESSAGE_QUEUE_DIR_new_queue(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t* thread) {
    /* first, create the new queue */
    JMESSAGE_QUEUE* msg_queue = (JMESSAGE_QUEUE*) calloc(1, sizeof(JMESSAGE_QUEUE));
    if (!msg_queue) {
        printf("Failed to allocate new message queue.\n");
        return 1;
    }

    msg_queue->list = JLIST_new();

    if (!msg_queue->list) {
        printf("JMESSAGE_QUEUE_DIR_new_queue: Error creating list.\n");
        return 1;
    }

    /* Associate the given thread id with the new queue in the directory*/
    if (JHASHMAP_add(queue_dir, (void*) thread, (void*) msg_queue)) {
        printf("Failed to add new queue to directory.\n");
        return 1;
    }

    return 0;
}

JMESSAGE_QUEUE* JMESSAGE_QUEUE_DIR_get_queue(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t* thread) {
    return JHASHMAP_get(queue_dir, (void*)thread);
}


int JTHREAD_IPC_send(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t* thread, void* message) {
    if (!queue_dir) {
        printf("JTHREAD_IPC_send: thread message queue directory is null.\n");
        return 1;
    }
    if (!thread) {
        printf("JTHREAD_IPC_send: receiving thread is null.\n");
        return 1;
    }

    JMESSAGE_QUEUE* queue = JMESSAGE_QUEUE_DIR_get_queue(queue_dir, thread);
    if (!queue) {
        printf("JTHREAD_IPC_send: Thread's queue does not exist.\n");
        return 1;
    }

    JMESSAGE* message_container = (JMESSAGE*) calloc(1, sizeof(JMESSAGE));
    if (!message_container) {
        printf("JTHREAD_IPC_send: message container could not be allocated.\n");
        return 1;
    }

    /* Put contents inside message container */
    message_container->message = message;
    message_container->sender = pthread_self();

    if (JLIST_prepend(queue->list, (void*) message_container)) {
        printf("whoops.");
    }

    /* then sleep*/

    return 0;
}

JTHREAD_IPC_receive


int hello(void) {
    printf("this is thread %ld\n", pthread_self());
    return 0;
}


pthread_t test1;
pthread_t test2;
int main(void) {
    JMESSAGE_QUEUE_DIR* queue_dir = JMESSAGE_QUEUE_DIR_new();
    pthread_create(&test1, NULL, hello, NULL);
    JMESSAGE_QUEUE_DIR_new_queue(queue_dir, &test1);
    pthread_create(&test2, NULL, hello, NULL);
    JMESSAGE_QUEUE_DIR_new_queue(queue_dir, &test2);
    pthread_join(test1, NULL);
    pthread_join(test2, NULL );
    JMESSAGE_QUEUE* queue1 = JMESSAGE_QUEUE_DIR_get_queue(queue_dir, &test1);
    if (!queue1) {
        printf("test1's queue doesn't exist.\n");
    }
    JMESSAGE_QUEUE* queue2 = JMESSAGE_QUEUE_DIR_get_queue(queue_dir, &test2);
    if (!queue2) {
        printf("test2's queue doesn't exist.\n");
    }

    char* message = "This is a new message\n";
    if (JTHREAD_IPC_send(queue_dir, &test1, (void*) message)) {
        printf("Issue sending.\n");
    }

    JMESSAGE_QUEUE* t1q = JMESSAGE_QUEUE_DIR_get_queue(queue_dir, &test1);
    JMESSAGE* messagebox = (JMESSAGE*) JLIST_pop(t1q->list);
    printf("%s\n", (char*) messagebox->message);
}