#ifndef JHASHMAP_H
#define JHASHMAP_H

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#define INITIAL_CAPACITY 20


typedef struct JHASHMAP_ENTRY {
    bool in_use;
    bool previously_in_use;
    void* key;
    void* value;
} JHASHMAP_ENTRY;

typedef struct JHASHMAP {
    /* a pointer to a user defined hash function for whatever data-type
     the key is */
    long (*hash_func)(void* key, size_t map_capacity);

    /* Function needed to compare keys of some data type*/
    bool (*key_compare_func)(void* key1, void* key2);

    /* The array that will hold the entries*/
    JHASHMAP_ENTRY* vector;

    /* the current number of occupied indexes of the vector */
    size_t occupied;

    /* the current capacity of the vector */
    size_t capacity;

} JHASHMAP;

/* 
    Create a new heap allocated hashmap.
    Must supply functions to hash keys and compare keys 
*/
JHASHMAP* JHASHMAP_new(long (*hash_func) (void* key, size_t map_capcity), bool (*key_compare_func) (void* key1, void* key2));

/*
    Add a heap allocated element to the hashmap.
    Returns
    0 - success
    TODO
*/
int8_t JHASHMAP_add(JHASHMAP* map, void* key, void* value);

/*
    Get value from hashmap for corresponding key. Returns NULL if key
    isn't in map.
*/
void* JHASHMAP_get(JHASHMAP* map, void* key);

/*
    Checks to see if key is in map. Returns true if so, false otherwise
*/
bool JHASHMAP_has(JHASHMAP* map, void* key);
   
/*
    Used if
*/
long JHASHMAP_quadradic_probe(JHASHMAP* map, void* key, size_t index, size_t capacity);

long JHASHMAP_hash_data(JHASHMAP* map, void* key, void* value);

int8_t grow_table(JHASHMAP* map);

/* TYPE SPECIFIC FUNCTIONS ################################## */
long JHASHMAP_hash_int(void* key, size_t map_capacity);
bool JHASHMAP_compare_int(void* key1, void* key2);


#endif