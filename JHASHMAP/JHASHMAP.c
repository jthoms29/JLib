
#include <JHASHMAP.h>

bool quadratic_probe(JHASHMAP* map, void* key, size_t initial_index, size_t* found_index, bool search_exact);
int grow_table(JHASHMAP* map);



JHASHMAP* JHASHMAP_new(size_t (*hash_func) (void* key, size_t map_capacity),
bool (*key_compare_func) (void* key1, void* key2)) {
    /* Allocate a new hashmap*/
    JHASHMAP* new_map = (JHASHMAP*) malloc(sizeof(JHASHMAP));
    if (!new_map) {
        perror("error allocating new hashmap");
        return NULL;
    }

    /* allocate the hashmap's vector */
    JHASHMAP_ENTRY* new_vector = (JHASHMAP_ENTRY*) calloc(INITIAL_CAPACITY, sizeof(JHASHMAP_ENTRY));

    if (!new_vector) {
        perror("error allocating hashmap's vector");
        free(new_map);
        return NULL;
    }

    new_map->vector = new_vector;
    new_map->hash_func = hash_func;
    new_map->key_compare_func = key_compare_func;
    new_map->capacity = INITIAL_CAPACITY;

    return new_map;
}




/*
 * Adds specified value to the hashmap using the specified key. If a value with an
 * identical key is already in the map, it is replaced.
 */
int JHASHMAP_add(JHASHMAP* map, void* key, void* value) {
    if (map == NULL) {
        return 1;
    }

    if (key == NULL) {
        return 1;
    }

    size_t index;
    /* Loop here so index retrieval can be re-attempted if hashmap needs to be resized. */
    for(;;) {
        /* get index from key */
        index = map->hash_func(key, map->capacity);

        // this space is already occupied and doesn't have the same key. Use quadratic probing to find an empty one
        if (map->vector[index].in_use && !map->key_compare_func(key, map->vector[index].key)) {

            // search for an empty index
            if (!quadratic_probe(map, key, index, &index, false)) {
            // If the above function returns false a different index wasn't able to be found.
            // The size of the vector must be increased
                grow_table(map);
            }

            else {
                break;
            }
        }
        else {
            break;
        }
    }

    /* Add value with corresponding key to map */
    map->vector[index].key = key;
    map->vector[index].value = value;
    map->vector[index].in_use = true;
    map->occupied++;

    /* If the load factor exceeds .75, increase the size of the table*/
    double load_factor = (double) map->occupied / (double) map->capacity;

    if (load_factor > 0.75) {
        grow_table(map);
    }

    return 0;
}


void* JHASHMAP_get(JHASHMAP* map, void* key) {
    if (map == NULL) {
        return NULL;
    }
    if (key == NULL) {
        return NULL;
    }


    /* Get the index from the supplied key*/
    size_t index = map->hash_func(key, map->capacity);

    /* If the keys match, return the item at this index*/
    if (map->key_compare_func(key, map->vector[index].key)) {
        return map->vector[index].value;
    }

    // Otherwise, need to do quadratic probing
    else {
        if (quadratic_probe(map, key, index, &index, true)) {
            return map->vector[index].value;
        }
        // key not in map
        return NULL;
    }

}


bool JHASHMAP_has(JHASHMAP* map, void* key) {
    if (map == NULL) {
        return false;
    }
    if (key == NULL) {
        return false;
    }


    /* Get index from supplied key*/
    size_t index = map->hash_func(key, map->capacity);

    /* If the keys match, return true */
    if (map->vector[index].in_use && map->key_compare_func(key, map->vector[index].key)) {
        return true;
    }

    /* Otherwise, need to do quadratic probing*/
    return quadratic_probe(map, key, index, &index, true);

}



/******* PRIV HELPER FUNCTIONS *********/

/*
 * Uses quadratic probing to search through the hashmap, either for an empty space or an exact key
 * @param[in] map - the hashmap
 * @param[in] key - key to search for
 * @param[in] initial_index - index to start probing from
 * @param[out] found_indx - reference to size_t variable to write found index to
 * @param[in] exact_search - denotes wether to search for an empty space or an exact value
 * \return true if valid index found, false otherwise
 */
bool quadratic_probe(JHASHMAP* map, void* key, size_t initial_index, size_t* found_index, bool search_exact) {
    /* offset from starting position */
    size_t j = 1;
    /* Number of checks. Will resize if reaches size of hashmap*/
    size_t count = 0;

    /* quadratic output based on offset from starting position*/
    long p_j;

    size_t new_index;
    /* check alternating offsets of perfect squares from the found index until an empty space is found*/
    for(;;) {
        /* the fraction in the second power is supposed to be int division*/
        p_j = pow(-1, (j-1)) *  pow(((j+1)/2), 2);

        new_index = (initial_index + p_j) % map->capacity;

        //found an empty index. return if ok
        if (!map->vector[new_index].in_use && !map->vector[new_index].previously_in_use) {
            if (search_exact)  { return false; }
            *found_index = new_index;
            return true;
        }
        /* check if occupied space has same key. If so, the index can be returned*/
        if (map->key_compare_func(key, map->vector[new_index].key)) {
            *found_index = new_index;
            return true;
        }

        j+=1;
        count+=1;
        /* Free index not found */
        if (count == map->capacity) {
            return false;
        }
    }
}

/* Increases size of hashmap if load factor exceeds a certain value*/
int grow_table(JHASHMAP* map) {
    size_t i, old_capacity, new_capacity;
    JHASHMAP_ENTRY* old_vector;
   
    old_capacity = map->capacity;
    new_capacity = old_capacity * 2;

    old_vector = map->vector;

    /* check for overflow*/
    if (new_capacity / 2 != old_capacity) {
        return 1;
    }

    JHASHMAP_ENTRY* new_vector = (JHASHMAP_ENTRY*) calloc(new_capacity, sizeof(JHASHMAP_ENTRY));
    if (!new_vector) {
        perror("could not allocate resized hashmap:");
        return 1;
    }

    map->capacity = new_capacity;
    map->occupied = 0;
    map->vector = new_vector;

    /* Rehash all vector elements from the original table into the new one*/
    for(i = 0; i < old_capacity; i++) {
        if (old_vector[i].in_use) {
            void* old_key = old_vector[i].key;
            void* old_value = old_vector[i].value;

            size_t index = map->hash_func(old_key, map->capacity);

            // this space is already occupied and doesn't have the same key. Use quadratic probing to find an empty one
            if (map->vector[index].in_use) {
                quadratic_probe(map, old_key, index, &index, false);
            }

            map->vector[index].key = old_key;
            map->vector[index].value = old_value;
            map->vector[index].in_use = true;
            map->occupied++;
        }
    }


    /* get rid of old vector*/
    free(old_vector);
    old_vector = NULL;
    
    return 0;
}


/***************************************/