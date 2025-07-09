#ifndef JVEC_H
#define JVEC_H

///@file JVEC.h

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


/// The initial number of space allocated for the vector
#define INITIAL_CAP 10

/**
 * The vector data structure
 */
typedef struct JVEC {
    /// Address of array contained in the vector
    void** head;

    /// The current length of the vector 
    size_t length;

    /// The current capacity of the vector
    size_t capacity;

    // thread synchronization variables
    pthread_mutex_t vec_tex;
    pthread_cond_t vec_cond;
    int readers;

} JVEC;

/**
 * Initializes a new JVEC. Returns a reference to the heap allocated vector.
 * @param[out] vector
 */
JVEC* JVEC_new(void);


/**
 * append a pointer to a variable to the end of the vector
 */
int JVEC_append(JVEC *vector, void* data_ptr);


/**
 * Prepend a variable to the start of the vector
 */
int JVEC_prepend(JVEC *vector, void* data_ptr);

/*
 * add a pointer to a variable to a valid index of the vector. If
 * an element already exists there, it is moved forward. If not (the first open position),
 * it is simply appended like normal.
 */
int JVEC_insert_at(JVEC* vector, void* data_ptr, size_t index);

/**
 * Get the element residing at the given index in the vector. Returns NULL on failure.
 */
void* JVEC_get_at(JVEC* vector, size_t index);



int JVEC_remove_at(JVEC *vector, void* data_ptr);


size_t JVEC_len(JVEC* vector);

int JVEC_free(JVEC* vector);


#endif