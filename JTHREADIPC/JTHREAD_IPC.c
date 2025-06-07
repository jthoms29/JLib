#include <JTHREAD_IPC.h>

pthread_t thread_table[2];
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
int hello(void) {
    printf("this is thread %ld\n", pthread_self());
    return 0;
}

int main(void) {
    long id = pthread_create(thread_table, NULL, hello, NULL);
    long id2 = pthread_create(thread_table+1, NULL, hello, NULL);
    printf("%ld, %ld \n", thread_table[0], thread_table[1]);
    pthread_join(thread_table[0], NULL);
    pthread_join(thread_table[1],NULL );
}