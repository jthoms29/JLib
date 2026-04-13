#ifndef JHASHSET_H
#define JHASHSET_H

/// @file JHASHSET.h
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define INITIAL_CAPACITY 1024 ///< Initial number of elements that can be held in set
#define EMPTY  0x0 // Indicates that an entry is empty
#define IN_USE 0x1 // Indicates that an entry is currently in use
#define TOMB   0x2 // Indicates that an entry was previously in use -> used for probing

// used in the set resize function
#define SHRINK 0
#define GROW   1

/**
 * An entry in the hashset data structure. Holds value
 */
typedef struct JHASHSET_ENTRY {
    /// Value for the entry, data held
    void* val;
    // current state of the entry, macros defined above
    uint8_t state;
} JHASHSET_ENTRY;


/**
 * The hashset data structure. Holds a vector of hashset entries.
 */
typedef struct JHASHSET {
    /// A pointer to a user defined hash function for whatever data-type the value is 
    size_t (*hash_func)(void* value);

    /// Function needed to compare values of some data type
    bool (*val_compare_func)(void* val1, void* val2);

    /// The array that will hold the entries
    JHASHSET_ENTRY* vector;

    /// the current number of occupied indices of the vector
    size_t occupied;

    /// the current capacity of the vector
    size_t capacity;
} JHASHSET;

/**
 * Create a new hashset. Must supply functions to hash values and compare values
 * @param[in] hash_func
 * @param[in] value_compare_func
 * \return A new heap allocated hashset
 */
JHASHSET* JHASHSET_new(size_t (*hash_func) (void* value), bool (*value_compare_func) (void* value1, void* value2));

/**
 * Frees the hashset data structure. Values should be freed by user.
 */
void JHASHSET_free(JHASHSET** set);

/**
 * Add a value to the hashset.
 * @param[in, out] set
 * @param[in] value
 * \return 0 on success, anything else on failure
 */
int JHASHSET_add(JHASHSET* set, void* val);



/**
 * Checks if provided value is in set
 * @param[in] set
 * @param[in] value
 */
bool JHASHSET_has(JHASHSET* set, void* value);
   

#endif
