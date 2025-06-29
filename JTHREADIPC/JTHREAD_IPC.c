#include <JTHREAD_IPC.h>
#include <pthread.h>
#include <stdint.h>

long JMESSAGE_QUEUE_DIR_hash_pthread_t(void* key, size_t map_capacity) {
    return (pthread_t) key % map_capacity;
}

bool JMESSAGE_QUEUE_DIR_compare_pthread_t(void* key1, void* key2) {
    return (pthread_t) key1 == (pthread_t) key2;
}



JMESSAGE_QUEUE_DIR* JMESSAGE_QUEUE_DIR_new(void) {
    JMESSAGE_QUEUE_DIR* queue_dir = (JMESSAGE_QUEUE_DIR*) calloc(1, sizeof(JMESSAGE_QUEUE_DIR));


    /* Create a new hashmap. */
    JHASHMAP* map = JHASHMAP_new(
        JMESSAGE_QUEUE_DIR_hash_pthread_t, 
        JMESSAGE_QUEUE_DIR_compare_pthread_t );

    queue_dir->map = map;
    pthread_mutex_init(&queue_dir->init_tex, NULL);
    pthread_cond_init(&queue_dir->init_cond, NULL);

    return queue_dir;
}


int JMESSAGE_QUEUE_DIR_new_queue(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t thread) {
    /* first, create the new queue */
    JMESSAGE_QUEUE* msg_queue = (JMESSAGE_QUEUE*) calloc(1, sizeof(JMESSAGE_QUEUE));
    if (!msg_queue) {
        printf("Failed to allocate new message queue.\n");
        return 1;
    }

    /* Init synchronization vars */
    pthread_mutex_init(&msg_queue->mq_tex, NULL);
    pthread_cond_init(&msg_queue->mq_cond, NULL);


    msg_queue->list = JLIST_new();

    if (!msg_queue->list) {
        printf("JMESSAGE_QUEUE_DIR_new_queue: Error creating list.\n");
        return 1;
    }

    /* Associate the given thread id with the new queue in the directory*/
    if (JHASHMAP_add(queue_dir->map, (void*) thread, (void*) msg_queue)) {
        printf("Failed to add new queue to directory.\n");
        return 1;
    }
  
    return 0;
}

JMESSAGE_QUEUE* JMESSAGE_QUEUE_DIR_get_queue(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t thread) {
    return JHASHMAP_get(queue_dir->map, (void*)thread);
}


int JTHREAD_IPC_send(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t thread, void* message) {
    if (!queue_dir) {
        printf("JTHREAD_IPC_send: thread message queue directory is null.\n");
        return 1;
    }
    if (!thread) {
        printf("JTHREAD_IPC_send: receiving thread is null.\n");
        return 1;
    }

    JMESSAGE_QUEUE* own_queue = JMESSAGE_QUEUE_DIR_get_queue(queue_dir, pthread_self());
    if (!own_queue) {
        printf("JTHREAD_IPC_send: sending thread must have its own queue.\n");
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

    /* Add to the reciever thread's queue*/
    pthread_mutex_lock(&queue->mq_tex);
    if (JLIST_prepend(queue->list, (void*) message_container)) {
        printf("whoops.");
    }
    queue->num_messages++;
    pthread_cond_signal(&queue->mq_cond);
    pthread_mutex_unlock(&queue->mq_tex);


    /* then sleep*/
    pthread_mutex_lock(&own_queue->mq_tex);
    if (own_queue->num_messages == 0) {
       pthread_cond_wait(&own_queue->mq_cond, &own_queue->mq_tex);
    }
    pthread_mutex_unlock(&own_queue->mq_tex);

    return 0;
}

int JTHREAD_IPC_reply(JMESSAGE_QUEUE_DIR* queue_dir, pthread_t thread, void* message) {
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

    /* Add to the reciever thread's queue*/
    pthread_mutex_lock(&queue->mq_tex);
    if (JLIST_prepend(queue->list, (void*) message_container)) {
        printf("whoops.");
    }
    queue->num_messages++;
    pthread_cond_signal(&queue->mq_cond);
    pthread_mutex_unlock(&queue->mq_tex);

    return 0;
}

void JTHREAD_IPC_thread_init(JMESSAGE_QUEUE_DIR* queue_dir) {
    pthread_mutex_lock(&queue_dir->init_tex);
    if (!queue_dir->started) {
        pthread_cond_wait(&queue_dir->init_cond, NULL);
    }
    pthread_mutex_unlock(&queue_dir->init_tex);
}

JMESSAGE* JTHREAD_IPC_get_message(JMESSAGE_QUEUE_DIR* queue_dir) {
    if (!queue_dir) {
        printf("JTHREAD_IPC_send: thread message queue directory is null.\n");
        return NULL;
    }

    JMESSAGE_QUEUE* own_queue = JMESSAGE_QUEUE_DIR_get_queue(queue_dir, pthread_self());
    if (!own_queue) {
        printf("JTHREAD_IPC_send: Thread's queue does not exist.\n");
        return NULL;
    }

    JMESSAGE* message;
    pthread_mutex_lock(&own_queue->mq_tex);
    if (own_queue->num_messages == 0) {
        message = NULL;
    }
    else {
        message = (JMESSAGE*) JLIST_pop(own_queue->list);
        own_queue->num_messages--;
    }
    pthread_mutex_unlock(&own_queue->mq_tex);
    return message;
}



void* hello(void*) {
    printf("this is thread %ld\n", pthread_self());
    return 0;
}

pthread_t test1;
pthread_t test2;

JMESSAGE_QUEUE_DIR* queue_dir;


void* test_send(void* id) {
    JTHREAD_IPC_thread_init(queue_dir);
    pthread_t main = (pthread_t) id;
    char* message = calloc(6, sizeof(char));
    strcpy(message, "hello");
    JTHREAD_IPC_send(queue_dir, main, (void*) message);
    return NULL;
}

void* test1_send(void*) {
    JTHREAD_IPC_thread_init(queue_dir);
    char* message = calloc(10, sizeof(char));
    strcpy(message, "hello t1");
    JTHREAD_IPC_send(queue_dir, test2, (void*) message);
    JMESSAGE* mes = JTHREAD_IPC_get_message(queue_dir);
    if (!mes) {
        printf("t1 message is null\n");
    }
    else {
        printf("%s\n", (char*) mes->message);
    }
}

void* test2_send(void* id) {
    JTHREAD_IPC_thread_init(queue_dir);
    char* message = calloc(10, sizeof(char));
    strcpy(message, "hello t2");
    JTHREAD_IPC_send(queue_dir, (pthread_t) id, (void*) message);

    JMESSAGE* mes = JTHREAD_IPC_get_message(queue_dir);
    if (!mes) {
        printf("t2 message is null\n");
    }
    else {
        printf("%s\n", (char*) mes->message);
    }

    JTHREAD_IPC_reply(queue_dir, test1, (void*) message);
}


int main(void) {
    queue_dir = JMESSAGE_QUEUE_DIR_new();
    pthread_mutex_lock(&queue_dir->init_tex);
    pthread_create(&test1, NULL, test1_send, (void*) pthread_self());
    pthread_create(&test2, NULL, test2_send, (void*) pthread_self());
    JMESSAGE_QUEUE_DIR_new_queue(queue_dir, pthread_self());
    JMESSAGE_QUEUE_DIR_new_queue(queue_dir, test1);
    JMESSAGE_QUEUE_DIR_new_queue(queue_dir, test2);
   
    queue_dir->started = 1;
    pthread_cond_signal(&queue_dir->init_cond);
    pthread_mutex_unlock(&queue_dir->init_tex);
    
   // pthread_join(test1, NULL);
    //pthread_join(test2, NULL );
    JMESSAGE_QUEUE* queue1 = JMESSAGE_QUEUE_DIR_get_queue(queue_dir, test1);
    if (!queue1) {
        printf("test1's queue doesn't exist.\n");
    }
    JMESSAGE_QUEUE* queue2 = JMESSAGE_QUEUE_DIR_get_queue(queue_dir, test2);
    if (!queue2) {
        printf("test2's queue doesn't exist.\n");

    }

    /*
    char* message = "This is a new message\n";
    if (JTHREAD_IPC_reply(queue_dir, test1, (void*) message)) {
        printf("Issue sending.\n");
    }

    char* message2 = "another message\n";
    if (JTHREAD_IPC_reply(queue_dir, test2, (void*) message2)) {
        printf("Issue sending.\n");
    }

    JMESSAGE_QUEUE* t1q = JMESSAGE_QUEUE_DIR_get_queue(queue_dir, test1);
    JMESSAGE* messagebox = (JMESSAGE*) JLIST_pop(t1q->list);
    printf("%s\n", (char*) messagebox->message);

    t1q = JMESSAGE_QUEUE_DIR_get_queue(queue_dir, test2);
    messagebox = (JMESSAGE*) JLIST_pop(t1q->list);
    printf("%s\n", (char*) messagebox->message);
    */

    pthread_join(test1, NULL);
    pthread_join(test2, NULL );
    /*
    JMESSAGE* mes;
    while ((mes = JTHREAD_IPC_get_message(queue_dir))) {
        printf("%s\n", (char*) mes->message);
    }
        */
}
