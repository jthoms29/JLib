#ifndef JHASHMAP_H
#define JHASHMAP_H

/// @file JHASHMAP.h
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define INITIAL_CAPACITY 1024 ///< Initial number of elements that can be held in map. MUST BE POWER OF 2!!
#define EMPTY  0x0 // Indicates that an entry is empty
#define IN_USE 0x1 // Indicates that an entry is currently in use
#define TOMB   0x2 // Indicates that an entry was previously in use -> used for probing

// used in the map resize function
#define SHRINK 0
#define GROW   1

/*
 * An entry in the hashmap data structure. Holds key-value pairs
 */
typedef struct JHASHMAP_ENTRY {
    // Key for the entry, what will be used to retrieve the data
    void* key;
    // Value for the entry, data held
    void* value;
    // Current state of this entry. Macros defined above
    uint8_t state;
} JHASHMAP_ENTRY;


/*
 * The hashmap data structure. Holds a vector of hashmap entries.
 */
typedef struct JHASHMAP {
    // A pointer to a user defined hash function for whatever data-type the key is 
    size_t (*hash_func)(void* key);
    // Function needed to compare keys of some data type
    bool (*key_compare_func)(void* key1, void* key2);
    // The array that will hold the entries
    JHASHMAP_ENTRY* vector;
    // the current number of occupied indices of the vector
    size_t occupied;
    // the current capacity of the vector
    size_t capacity;
} JHASHMAP;

/**
 * @brief Create a new hashmap.
 * @details Allocates and returns a new JHASHMAP. 
 * Functions to hash and compare keys must be supplied.
 * @note Must be freed using JHASHMAP_free
 * @param[in] hash_func Pointer to function used to hash keys
 * @param[in] key_compare_func Pointer to function used to hash values
 * @pre None
 * @post New JHASHMAP allocated on heap
 * @return Reference to new JHASHMAP
 */
JHASHMAP* JHASHMAP_new(size_t (*hash_func) (void* key), bool (*key_compare_func) (void* key1, void* key2));

/**
 * @brief Add a key-value pair to the hashmap.
 * @details Insert a value into the hashmap using the specified key.
 * @note Key must be type that supplied hash function works with.
 * @param[in, out] map JHASHMAP to be added to
 * @param[in] key Key used to index supplied value
 * @param[in] value Value to add to map
 * @pre map, key must be non-null, key must be able to be hashed by supplied hash function
 * @post Key value pair added to map. If pair with given key already exits,
 * it is replaced. If pair is new, occupied count is incremented. If load
 * factor increases enough from insertion, internal vector may resize
 * @return 0 on success, anything else on failure
 */
int JHASHMAP_add(JHASHMAP* map, void* key, void* value);

/**
 * @brief Remove key-value pair from hashmap
 * @details Remove key-value pair that supplied key is associated
 * with from hashmap.
 * @note Key must be type that supplied hash function works with.
 * @param[in, out] map JHASHMAP that pair will be removed from
 * @param[in] key Key used to index pair to be removed
 * @pre map, key must be non-null, key must be able to be hashed by supplied hash function
 * @post If pair indexed by key exists in map, it is removed and associated value is returned.
 * Occupied count decremented. If pair does not exist, nothing is done and NULL is returned
 * @return value previously associated with key if pair exists in map, NULL otherwise
 */
void* JHASHMAP_remove(JHASHMAP* map, void* key);

/** 
 * @brief Get value from hashmap for corresponding key
 * @details Retrieves reference to value associated with key from map, if exists
 * within map
 * @note Key must be type that supplied hash function works with.
 * @param[in] map JHASHMAP value will be retrieved from
 * @param[in] key Key associated with value to be retrieved
 * @pre map, key must be non-null, key must be able to be hashed by supplied hash function
 * @post None
 * @return Value that corresponds with key returned if pair exists in map,
 * NULL otherwise.
 */
void* JHASHMAP_get(JHASHMAP* map, void* key);

/**
 * @brief Checks if provided key exists in map
 * @details Returns boolean indicating if key is currently in map or not
 * @note Key must be type that supplied hash function works with.
 * @param[in] map JHASHMAP where existence of key will be checked
 * @param[in] key Key to check existence of in map
 * @pre map, key must be non-null, key must be able to be hashed by supplied hash function
 * @post None
 * @return true if key exists in map, false otherwise
 */
bool JHASHMAP_has(JHASHMAP* map, void* key);
   
/**
 * @brief Free supplied JHASHMAP
 * @details Frees internal vector of map as well as map itself.
 * @note Data entered into map (keys, values) are NOT freed.
 * @param[in, out] map_ptr Inderect pointer to allocated JHASHMAP
 * @pre map_ptr must be indirect pointer to previously allocated JHASHMAP
 * @post internal data structures and map itself are freed. Not data entered
 * into map is freed
 * @return None
 */
void JHASHMAP_free(JHASHMAP** map_ptr);
#endif
