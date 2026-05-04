#ifndef JVEC_H
#define JVEC_H

///@file JVEC.h

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


/// The initial number of space allocated for the vector
#define INITIAL_PAD 8
#define INITIAL_CAP 16

enum resize_action {
    SHRINK_LEFT,
    GROW_LEFT,
    SHRINK_RIGHT,
    GROW_RIGHT,
};




/**
 * The vector data structure
 */
typedef struct JVEC {
    // Address of array contained in the vector
    void** head;
    // first index taking the pad into account
    void** start;
    // The current len of the vector 
    size_t len;
    // The current capacity of the vector
    size_t cap;
    // current allocated pad (pre-allocated space before start of vector)    
    size_t pad_alloc;
    // user supplied free function to use on vector's items
    void (*free_func)(void* item);

} JVEC;

/**
 * Initializes a new JVEC. Returns a reference to the heap allocated vector.
 * \param[in] item_free_func Pointer to function that frees datatype held by this vector
 * \return Pointer to new vector on success, NULL on failure
 */
JVEC* JVEC_new( void (*item_free_func)(void* item) );


static inline void* JVEC_get(JVEC* vec, size_t idx) {
    return (idx < vec->len) ? vec->start[idx] : NULL;
}

/**
 * Append a variable to the end of the vector.
 * \param[out] vector A pointer to a previously allocated JVEC
 * \param[in] data_ptr Pointer to a piece of data you wish to append to the vector 
 * \return 0 on success, 1 on failure
 */
int JVEC_append(JVEC *vec, void* data_ptr);


/**
 * Prepend a variable to the start of the vector.
 * \param[out] vector A pointer to a previously allocated JVEC
 * \param[in] data_ptr Pointer to a piece of data you wish to prepend to the vector 
 * \return 0 on success, 1 on failure
 */
int JVEC_prepend(JVEC *vec, void* data_ptr);


int JVEC_in_after(JVEC* vec, void* data, size_t idx);

int JVEC_in_before(JVEC* vec, void* data, size_t idx);

/**
 * Get the element residing at the given index in the vector.
 * \param[out] vector A pointer to a previously allocated JVEC
 * \param[in] index The index of the item in the vector you wish to retrieve
 * \return A pointer to the item on success, NULL on failure.
 */
void* JVEC_get_at(JVEC* vec, size_t index);

/**
 * Remove the last element in the vector and return it.
 * \param[out] vector A pointer to a previously allocated JVEC
 * \return A pointer to the former last item in the vector on success, NULL on failure.
 */
void* JVEC_pop(JVEC* vec);

/**
 * Remove the element residing at the given index in the vector.
 * \param[out] vector A pointer to a previously allocated JVEC
 * \param[in] index The index of the item in the vector you wish to remove
 * \return 0 on success, 1 on failure
 */
int JVEC_remove_at(JVEC *vec, void* data_ptr);

/**
 * Get the current len of the given vector.
 \param[in] vector A pointer to a previously allocated JVEC
 \return len on success, -1 on failure
 */
long JVEC_len(JVEC* vec);

/**
 * Free the specified JVEC. Uses free function passed in on intitialization.
 * \param[in] vector A pointer to the JVEC you wish to free
 */
void JVEC_free(JVEC** vec);


#endif