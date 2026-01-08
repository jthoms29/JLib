
#include <JHASHSET.h>

bool quadratic_probe(JHASHSET* set, void* value, size_t initial_index, size_t* found_index, bool search_exact);
int grow_table(JHASHSET* set);



JHASHSET* JHASHSET_new(size_t (*hash_func) (void* value, size_t set_capacity),
bool (*value_compare_func) (void* value1, void* value2)) {
    /* Allocate a new hashset*/
    JHASHSET* new_set = (JHASHSET*) malloc(sizeof(JHASHSET));
    if (!new_set) {
        perror("error allocating new hashset");
        return NULL;
    }

    /* allocate the hashset's vector */
    JHASHSET_ENTRY* new_vector = (JHASHSET_ENTRY*) calloc(INITIAL_CAPACITY, sizeof(JHASHSET_ENTRY));

    if (!new_vector) {
        perror("error allocating hashset's vector");
        free(new_set);
        return NULL;
    }

    new_set->vector = new_vector;
    new_set->hash_func = hash_func;
    new_set->value_compare_func = value_compare_func;
    new_set->capacity = INITIAL_CAPACITY;

    return new_set;
}

int JHASHSET_free(JHASHSET* set) {
    free(set->vector);
    set->vector = NULL;
    free(set);
    return 0;
}




/*
 * Adds specified value to the hashset.
 */
int JHASHSET_add(JHASHSET* set, void* value) {
    if (set == NULL) {
        return 1;
    }
    if (value == NULL) {
        return 1;
    }

    size_t index;
    /* Loop here so index retrieval can be re-attempted if hashset needs to be resized. */
    for(;;) {
        /* get index from value */
        index = set->hash_func(value, set->capacity);

        // this space is already occupied and doesn't have the same value. Use quadratic probing to find an empty one
        if (set->vector[index].in_use && !set->value_compare_func(value, set->vector[index].value)) {

            // search for an empty index
            if (!quadratic_probe(set, value, index, &index, false)) {
            // If the above function returns false a different index wasn't able to be found.
            // The size of the vector must be increased
                grow_table(set);
            }

            else {
                break;
            }
        }
        else {
            break;
        }
    }

    /* Add value with corresponding value to set */
    set->vector[index].value = value;
    set->vector[index].in_use = true;
    set->occupied++;

    /* If the load factor exceeds .75, increase the size of the table*/
    double load_factor = (double) set->occupied / (double) set->capacity;

    if (load_factor > 0.75) {
        grow_table(set);
    }
    return 0;
}



bool JHASHSET_has(JHASHSET* set, void* value) {
    if (set == NULL) {
        return false;
    }
    if (value == NULL) {
        return false;
    }


    /* Get index from supplied value*/
    size_t index = set->hash_func(value, set->capacity);

    /* If the values match, return true */
    if (set->vector[index].in_use && set->value_compare_func(value, set->vector[index].value)) {
        return true;
    }

    /* Otherwise, need to do quadratic probing*/
    return quadratic_probe(set, value, index, &index, true);

}



/******* PRIV HELPER FUNCTIONS *********/

/*
 * Uses quadratic probing to search through the hashset, either for an empty space or an exact value
 * @param[in] set - the hashset
 * @param[in] value - value to search for
 * @param[in] initial_index - index to start probing from
 * @param[out] found_indx - reference to size_t variable to write found index to
 * @param[in] exact_search - denotes wether to search for an empty space or an exact value
 * \return true if valid index found, false otherwise
 */
bool quadratic_probe(JHASHSET* set, void* value, size_t initial_index, size_t* found_index, bool search_exact) {
    /* offset from starting position */
    size_t j = 1;
    /* Number of checks. Will resize if reaches size of hashset*/
    size_t count = 0;

    /* quadratic output based on offset from starting position*/
    long p_j;

    size_t new_index;
    /* check alternating offsets of perfect squares from the found index until an empty space is found*/
    for(;;) {
        /* the fraction in the second power is supposed to be int division*/
        p_j = pow(-1, (j-1)) *  pow(((j+1)/2), 2);

        new_index = (initial_index + p_j) % set->capacity;

        //found an empty index. return if ok
        if (!set->vector[new_index].in_use && !set->vector[new_index].previously_in_use) {
            if (search_exact)  { return false; }
            *found_index = new_index;
            return true;
        }
        /* check if occupied space has same value. If so, the index can be returned*/
        if (set->value_compare_func(value, set->vector[new_index].value)) {
            *found_index = new_index;
            return true;
        }

        j+=1;
        count+=1;
        /* Free index not found */
        if (count == set->capacity) {
            return false;
        }
    }
}

/* Increases size of hashset if load factor exceeds a certain value*/
int grow_table(JHASHSET* set) {
    size_t i, old_capacity, new_capacity;
    JHASHSET_ENTRY* old_vector;
   
    old_capacity = set->capacity;
    new_capacity = old_capacity * 2;

    old_vector = set->vector;

    /* check for overflow*/
    if (new_capacity / 2 != old_capacity) {
        return 1;
    }

    JHASHSET_ENTRY* new_vector = (JHASHSET_ENTRY*) calloc(new_capacity, sizeof(JHASHSET_ENTRY));
    if (!new_vector) {
        perror("could not allocate resized hashset:");
        return 1;
    }

    set->capacity = new_capacity;
    set->occupied = 0;
    set->vector = new_vector;

    /* Rehash all vector elements from the original table into the new one*/
    for(i = 0; i < old_capacity; i++) {
        if (old_vector[i].in_use) {
            void* old_value = old_vector[i].value;

            size_t index = set->hash_func(old_value, set->capacity);

            // this space is already occupied and doesn't have the same value. Use quadratic probing to find an empty one
            if (set->vector[index].in_use) {
                quadratic_probe(set, old_value, index, &index, false);
            }

            set->vector[index].value = old_value;
            set->vector[index].value = old_value;
            set->vector[index].in_use = true;
            set->occupied++;
        }
    }


    /* get rid of old vector*/
    free(old_vector);
    old_vector = NULL;
    
    return 0;
}


/***************************************/