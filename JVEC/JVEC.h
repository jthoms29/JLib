#ifndef JVEC_H
#define JVEC_H

#include <stdlib.h>

#define INITIAL_CAP 10

typedef struct JVEC {
    // address of the first element in the vector
    void** head;

    // the current length of the vector 
    size_t length;

    // the current capacity of the vector
    size_t capacity;
} JVEC;

/*
 * initialize a new JVEC. Returns a reference to the heap allocated vector.
 */
JVEC* JVEC_new(void);

/*
 * append a pointer to a heap allocated variable to the end of the vector
 */
int JVEC_append(JVEC *vector, void* data_ptr);


/*
 * prepend a pointer to a heap allocated variable to the start of the vector
 */
int JVEC_prepend(JVEC *vector, void* data_ptr);

/*
 * add a pointer to a heap allocated variable to a valid index of the vector. If
 * an element already exists there, it is moved forward. If not (the first open posisition),
 * it is simply appended like normal.
 */
int JVEC_insert_at(JVEC* vector, void* data_ptr, size_t index);



int JVEC_remove_at(JVEC *vector, void* data_ptr);


size_t JVEC_len(JVEC* vector);

int JVEC_free(JVEC* vector);






#endif