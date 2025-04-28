#ifndef JHASHMAP_H
#define JHASHMAP_H

#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define INITIAL_CAPACITY 20

typedef struct JHASHMAP {
    /* a pointer to a user defined hash function for whatever data-type
     the key is */
    size_t (*hash_func)(void* key, size_t map_capacity);

    /* address of the first element in the vector */
    void** vector;

    /* the current number of occupied indexes of the vector */
    size_t occupied;

    /* the current capacity of the vector */
    size_t capacity;
} JHASHMAP;


JHASHMAP* JHASHMAP_new(size_t (*hash_func));

size_t JHASHMAP_string_hash(char* strn, size_t map_capacity);


size_t JHASHMAP_int_hash(int64_t num, size_t map_capacity);
    



size_t JHASHMAP_quadradic_probe(void** vector, size_t index, size_t capacity);


size_t JHASHMAP_hash_data(JHASHMAP* map, void* key, void* value);

#endif