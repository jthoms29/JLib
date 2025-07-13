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

    /// Mutex variable used to access the vector in each function
    pthread_mutex_t vec_tex;

    /// Condition variable. If a thread calls a function that modifies the
    /// vector but there are currently other threads reading from the vector,
    /// it will wait on this variable.
    pthread_cond_t vec_cond;

    /// The number of threads currently reading from the vector. Since certain
    /// functions don't modify the vector, multiple threads are able to call these
    /// functions at a time.
    int readers;

} JVEC;

/**
 * Initializes a new JVEC. Returns a reference to the heap allocated vector.
 */
JVEC* JVEC_new(void);


/**
 * Append a variable to the end of the vector.
 * \param[out] vector A pointer to a previously allocated JVEC
 * \param[in] data_ptr Pointer to a piece of data you wish to append to the vector 
 * \return 0 on success, 1 on failure
 */
int JVEC_append(JVEC *vector, void* data_ptr);


/**
 * Prepend a variable to the start of the vector.
 * \param[out] vector A pointer to a previously allocated JVEC
 * \param[in] data_ptr Pointer to a piece of data you wish to prepend to the vector 
 * \return 0 on success, 1 on failure
 */
int JVEC_prepend(JVEC *vector, void* data_ptr);

/**
 * Add a pointer to a variable to a valid index of the vector. If
 * an element already exists there, That element and all proceeding are moved forward.
 * \param[out] vector A pointer to a previously allocated JVEC
 * \param[in] data_ptr Pointer to a piece of data you wish to insert into to the vector 
 * \return 0 on success, 1 on
 */
int JVEC_insert_at(JVEC* vector, void* data_ptr, size_t index);

/**
 * Get the element residing at the given index in the vector.
 * \param[out] vector A pointer to a previously allocated JVEC
 * \param[in] index The index of the item in the vector you wish to retrieve
 * \return A pointer to the item on success, NULL on failure.
 */
void* JVEC_get_at(JVEC* vector, size_t index);


/**
 * Remove the element residing at the given index in the vector.
 * \param[out] vector A pointer to a previously allocated JVEC
 * \param[in] index The index of the item in the vector you wish to remove
 * \return 0 on success, 1 on failure
 */
int JVEC_remove_at(JVEC *vector, void* data_ptr);

/**
 * Get the current length of the given vector.
 \param[in] vector A pointer to a previously allocated JVEC
 \return length on success, -1 on failure
 */
long JVEC_len(JVEC* vector);


int JVEC_free(JVEC* vector);


#endif