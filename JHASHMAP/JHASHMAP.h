#ifndef JHASHMAP_H
#define JHASHMAP_H

/// @file JHASHMAP.h
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define INITIAL_CAPACITY 128 ///< Initial number of elements that can be held in map
#define EMPTY  0x0
#define IN_USE 0x1
#define TOMB   0x2

/**
 * An entry in the hashmap data structure. Holds key-value pair
 */
typedef struct JHASHMAP_ENTRY {
    /// Key for the entry, what will be used to retrieve the data
    void* key;
    /// Value for the entry, data held
    void* value;
    uint8_t state;
    uint8_t pad[7];
} JHASHMAP_ENTRY;


/**
 * The hashmap data structure. Holds a vector of hashmap entries.
 */
typedef struct JHASHMAP {
    /// A pointer to a user defined hash function for whatever data-type the key is 
    size_t (*hash_func)(void* key);

    /// Function needed to compare keys of some data type
    bool (*key_compare_func)(void* key1, void* key2);

    /// The array that will hold the entries
    JHASHMAP_ENTRY* vector;

    /// the current number of occupied indices of the vector
    size_t occupied;

    /// the current capacity of the vector
    size_t capacity;
} JHASHMAP;

/**
 * Create a new hashmap. Must supply functions to hash keys and compare keys
 * @param[in] hash_func
 * @param[in] key_compare_func
 * \return A new heap allocated hashmap
 */
JHASHMAP* JHASHMAP_new(size_t (*hash_func) (void* key), bool (*key_compare_func) (void* key1, void* key2));

/**
 * Add a key-value pair to the hashmap.
 * @param[in, out] map
 * @param[in] key
 * @param[in] value
 * \return 0 on success, anything else on failure
 */
int JHASHMAP_add(JHASHMAP* map, void* key, void* value);


void* JHASHMAP_remove(JHASHMAP* map, void* key);

/** 
 * Get value from hashmap for corresponding key. Returns NULL if key
 * @param[in] map
 * @param[in] key
 * \return Value that corresponds to key, NULL if provided key not in map
 */
void* JHASHMAP_get(JHASHMAP* map, void* key);

/**
 * Checks if provided key is in map
 * @param[in] map
 * @param[in] key
 */
bool JHASHMAP_has(JHASHMAP* map, void* key);
   

void JHASHMAP_free(JHASHMAP** map_ptr);
#endif
