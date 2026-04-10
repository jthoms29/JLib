#ifndef JHASHSET_H
#define JHASHSET_H

/// @file JHASHSET.h
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define INITIAL_CAPACITY 20 ///< Initial number of elements that can be held in set


/**
 * An entry in the hashset data structure. Holds value
 */
typedef struct JHASHSET_ENTRY {
    /// Indicates if this entry is currently in use
    bool in_use;

    /// Indicates if this entry was previously in use, needed for probing
    bool previously_in_use;

    /// Value for the entry, data held
    void* value;
} JHASHSET_ENTRY;


/**
 * The hashset data structure. Holds a vector of hashset entries.
 */
typedef struct JHASHSET {
    /// A pointer to a user defined hash function for whatever data-type the value is 
    size_t (*hash_func)(void* value, size_t set_capacity);

    /// Function needed to compare values of some data type
    bool (*value_compare_func)(void* value1, void* value2);

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
JHASHSET* JHASHSET_new(size_t (*hash_func) (void* value, size_t set_capcity), bool (*value_compare_func) (void* value1, void* value2));

/**
 * Frees the hashset data structure. Values should be freed by user.
 */
int JHASHSET_free(JHASHSET* set);

/**
 * Add a value to the hashset.
 * @param[in, out] set
 * @param[in] value
 * \return 0 on success, anything else on failure
 */
int JHASHSET_add(JHASHSET* set, void* value);



/**
 * Checks if provided value is in set
 * @param[in] set
 * @param[in] value
 */
bool JHASHSET_has(JHASHSET* set, void* value);
   

#endif
