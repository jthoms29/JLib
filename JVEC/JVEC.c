#include <JVEC.h>
#include <pthread.h>

static int JVEC_grow(JVEC* vector) {
    size_t new_capacity = vector->capacity * 2;


    void** new_head = (void**) realloc(vector->head, new_capacity* sizeof(void*));
    if (!new_head) {
        perror("Failed to resize vector.");
        return 1;
    }

    vector->head = new_head;
    vector->capacity = new_capacity;
    return 0;
}

static int JVEC_shrink(JVEC* vector) {
    size_t new_capacity = vector->capacity / 2;
    printf("%ld new cap %ld\n", vector->capacity, new_capacity);

    void ** new_head = (void**) realloc(vector->head, new_capacity * sizeof(void*));
    if (!new_head) {
        perror("Failed to resize vector");
        return 1;
    }

    vector->head = new_head;
    vector->capacity = new_capacity;
    return 0;
}
/*
 * initialize a new JVEC. Returns a reference to the heap allocated vector.
 */
JVEC* JVEC_new( void (*item_free_func)(void* item) ) {
    JVEC* vector = (JVEC*) calloc(sizeof(JVEC), 1);

    if (!vector) {
        perror("Failed to allocate new vector");
        return NULL;
    }

    // start off with INITIAL_CAP elements
    void** array = (void**)calloc(sizeof(void*),INITIAL_CAP);
    // allocation fails
    if (!array) {
        perror("Failed to allocate array memory");
        return NULL;
    }

    // set new array to head
    vector->head = array;

    // no items currently in vector
    vector->length = 0;

    // starting capacity is 10
    vector->capacity = INITIAL_CAP;

    vector->free_func = item_free_func;

    //initialize synchronization variables
    pthread_mutex_init(&vector->vec_tex, NULL);
    pthread_cond_init(&vector->vec_cond, NULL);
    vector->readers = 0;

    return vector;
}


/*
 * append a pointer to a heap allocated variable to the end of the vector
 */
int JVEC_append(JVEC* vector, void* data_ptr) {

    if (!vector) {
        printf("JVEC_append: vector is NULL.\n");
        return 1;
    }

    //modifies vector, so must wait for all readers to leave
    pthread_mutex_lock(&vector->vec_tex);
    if (vector->readers) {
        pthread_cond_wait(&vector->vec_cond, &vector->vec_tex);
    }

    // set next open index to new element
    *(vector->head + vector->length) = data_ptr;

    vector->length++;

    // grow vector if needed
    if (vector->length == vector->capacity) {
        JVEC_grow(vector);
    }

    pthread_mutex_unlock(&vector->vec_tex);
    return 0;
}


/*
 * prepend a pointer to a heap allocated variable to the start of the vector
 */
int JVEC_prepend(JVEC* vector, void* data_ptr) {


    //modifies vector, so must wait for all readers to leave
    pthread_mutex_lock(&vector->vec_tex);
    if (vector->readers) {
        pthread_cond_wait(&vector->vec_cond, &vector->vec_tex);
    }

    // the index vector->length is one after the final element
    size_t i;
    for (i = vector->length; i > 0; i--) {
        // move previous element forward to current index
        *(vector->head + i) = *(vector->head + i-1);
    }

    *vector->head = data_ptr;

    vector->length++;

    if (vector->length == vector->capacity) {
        JVEC_grow(vector);
    }

    pthread_mutex_unlock(&vector->vec_tex);
    return 0;
}


/*
 * add a pointer to a heap allocated variable to a valid index of the vector. If
 * an element already exists there, it is moved forward. If not (the first open posisition),
 * it is simply appended like normal.
 */
int JVEC_insert_at(JVEC* vector, void* data_ptr, size_t index) {

    // if the index isn't somewhere in the middle or the first open position
    if (index > vector->length) {
        perror("Index out of bounds\n");
        return 1;
    }

    // move existing elements forward if needed
    size_t i;
    // the index vector->length is one after the final element
    for (i = vector->length; i > index; i--) {
        // move previous element forward to current index
        *(vector->head + i) = *(vector->head + i-1);
    }

    // insert new element at specified index
    *(vector->head+index) = data_ptr;

    vector->length+=1;

    //grow vector if needed
    if (vector->length == vector->capacity) {
        JVEC_grow(vector);
    }

    return 0;
}

void* JVEC_pop(JVEC* vector) {

    if (vector->length == 0) {
        return NULL;
    }

    void* item = *(vector->head + vector->length - 1);
    vector->length--;

    if (vector->length == vector->capacity/2) {
        JVEC_shrink(vector);
    }

    return item;
}

void* JVEC_get_at(JVEC* vector, size_t index) {

    // function only reads from vector, multiple threads can do this at a time.
    pthread_mutex_lock(&vector->vec_tex);
    vector->readers++;
    pthread_mutex_unlock(&vector->vec_tex);

    void* ret;

    if (index >= vector->length) {
        perror("Index out of bounds\n");
        ret = NULL;
    }
    else {
        ret =  *(vector->head + index);
    }

    pthread_mutex_lock(&vector->vec_tex);
    vector->readers--;
    //if this was the last reader, wake up a possibly sleeping writer
    if (vector->readers == 0) {
        pthread_cond_signal(&vector->vec_cond);
    }
    pthread_mutex_unlock(&vector->vec_tex);

    return ret;
}

long JVEC_len(JVEC* vector) {
    if (vector == NULL) {
        printf("JVEC_len: vector is NULL\n");
        return -1;
    }

    pthread_mutex_lock(&vector->vec_tex);
    vector->readers++;
    pthread_mutex_unlock(&vector->vec_tex);

    long len = vector->length;

    pthread_mutex_lock(&vector->vec_tex);
    vector->readers--;
    if (vector->readers == 0) {
        pthread_cond_signal(&vector->vec_cond);
    }
    pthread_mutex_unlock(&vector->vec_tex);

    return len;
}


void JVEC_free(JVEC *vector) {

}

