#ifndef JHASHMAP_H
#define JHASHMAP_H

#include <bits/pthreadtypes.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <pthread.h>

#define INITIAL_CAPACITY 20


typedef struct JHASHSET_ENTRY {
    bool in_use;
    bool previously_in_use;
    void* value;
} JHASHSET_ENTRY;

typedef struct JHASHSET {
    /* a pointer to a user defined hash function for whatever data-type
     the value is */
    long (*hash_func)(void* value, size_t map_capacity);

    /* Function needed to compare values of some data type*/
    bool (*value_compare_func)(void* value1, void* value2);

    /* The array that will hold the entries*/
    JHASHSET_ENTRY* vector;

    /* the current number of occupied indexes of the vector */
    size_t occupied;

    /* the current capacity of the vector */
    size_t capacity;

    /* SYNCHRONIZATION VARS  ******************************* */
    pthread_mutex_t set_tex;

    pthread_cond_t set_cond;

    // used to keep track of current number of readers. Functions that
    // modify the hashmap cannot be executed if this number isn't 0.
    int readers;
    

} JHASHSET;

/* 
    Create a new heap allocated hashset.
    Must supply functions to hash values and compare values
*/
JHASHSET* JHASHSET_new(long (*hash_func) (void* value, size_t map_capcity), bool (*value_compare_func) (void* value1, void* value2));

/*
    0 - success
    TODO
*/
int JHASHSET_add(JHASHSET* set, void* value);

/*
*/
void* JHASHSET_get(JHASHSET* set, void* value);

/*
*/
bool JHASHSET_has(JHASHSET* set, void* value);
   
/*
    Used if
*/
long JHASHSET_quadradic_probe(JHASHSET* set, void* value, size_t index, size_t capacity);

long JHASHSET_hash_data(JHASHSET* set, void* value);

int grow_set(JHASHSET* set);

/* TYPE SPECIFIC FUNCTIONS ################################## */
long JHASHSET_hash_int(void* key, size_t map_capacity);
bool JHASHSET_compare_int(void* key1, void* key2);


#endif
