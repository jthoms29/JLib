
#include <JHASHMAP.h>

bool quadratic_probe(JHASHMAP* map, void* key, size_t initial_index, size_t* found_index, bool search_exact);
int resize_table(JHASHMAP* map, bool grow);



JHASHMAP* JHASHMAP_new(size_t (*hash_func) (void* key), bool (*key_compare_func) (void* key1, void* key2)) {
    // Allocate a new hashmap
    JHASHMAP* new_map = (JHASHMAP*) calloc(1, sizeof(*new_map));
    if (!new_map) {
        perror("error allocating new hashmap");
        return NULL;
    }

    // allocate the hashmap's vector
    JHASHMAP_ENTRY* new_vector = (JHASHMAP_ENTRY*) calloc(INITIAL_CAPACITY, sizeof(*new_vector));

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
    if (!map || !key) {
        return -1;
    }

    size_t index;
    JHASHMAP_ENTRY* entry;

    // sorry big E.W.
    retry:
        /* get index from key */
        index = map->hash_func(key) % map->capacity;
        entry = map->vector+index;

        // this space is already occupied and doesn't have the same key. Use quadratic probing to find an empty one
        if(entry->state == IN_USE && !map->key_compare_func(key, entry->key)) {
            if( !quadratic_probe(map, key, index, &index, false)) {
                // unable to find suitable location in current map. Increase size of array
                resize_table(map, true);
                goto retry;
            }
        }

    /* Add value with corresponding key to map */
    entry = map->vector+index;
    entry->key = key;
    entry->value = value;
    // only increment num of elements if this isn't replacing a pre-existing element
    if (!(entry->state == IN_USE)) {
        map->occupied++;
        entry->state = IN_USE;
    }


    if (map->occupied*2 > map->capacity) {
        resize_table(map, true);
    }
    return 0;
}

void* JHASHMAP_remove(JHASHMAP* map, void* key) {
    if (!map || !key) {
        return NULL;
    }

    size_t index;
    JHASHMAP_ENTRY* entry;
    void* ret = NULL;

    index = map->hash_func(key) % map->capacity;
    entry = map->vector+index;

    if (entry->state == IN_USE && map->key_compare_func(entry->key, key)) {
        entry->state = TOMB;
        map->occupied--;
        ret = entry->value;
    }
    else if (entry->state & (IN_USE | TOMB) && quadratic_probe(map, key, index, &index, true)) {
        entry->state = TOMB;
        map->occupied--;
        ret = entry->value;
    }
    else {
        return NULL;
    }

    if (map->occupied * 2 < map->capacity) {
        resize_table(map, false);
    }
    return ret;
}

void* JHASHMAP_get(JHASHMAP* map, void* key) {
    if (!map || !key) {
        return NULL;
    }

    /* Get the index from the supplied key*/
    size_t index = map->hash_func(key) % map->capacity;
    JHASHMAP_ENTRY* entry = map->vector+index;
    
    /* If the keys match, return the item at this index*/
    if (entry->state == IN_USE && map->key_compare_func(key, map->vector[index].key)) {
        return map->vector[index].value;
    }

    // Otherwise, need to do quadratic probing
    if (entry->state & (TOMB | IN_USE) && quadratic_probe(map, key, index, &index, true)) {
        return map->vector[index].value;
    }
    // key not in map
    return NULL;

}


bool JHASHMAP_has(JHASHMAP* map, void* key) {
    if (map == NULL) {
        return false;
    }
    if (key == NULL) {
        return false;
    }


    /* Get index from supplied key*/
    size_t index = map->hash_func(key) % map->capacity;

    /* If the keys match, return true */
    if (map->vector[index].state & 0x1 && map->key_compare_func(key, map->vector[index].key)) {
        return true;
    }

    /* Otherwise, need to do quadratic probing*/
    return quadratic_probe(map, key, index, &index, true);

}

void JHASHMAP_free(JHASHMAP** map_ptr) {
    free((*map_ptr)->vector);
    free(*map_ptr);
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
    // offset from starting position
    size_t j = 1;
    // number of checks. Will resize if reaches size of hashmap
    size_t count = 0;
   
    JHASHMAP_ENTRY* entry;
    // check alternating offsets of perfect squares from the found index until an empty space is found
    while (count < map->capacity) {
        size_t k = (j + 1)/2; // 1,1,2,2,3,3,...
        size_t offset = k*k % map->capacity;


        size_t new_idx = (j % 2 == 0) 
            ? (initial_index + offset) % map->capacity
            : (initial_index + map->capacity - offset) % map->capacity; // properly handle negative modulo


        entry = map->vector+new_idx;
        //found an empty index. return if ok
        if (!(entry->state & IN_USE)) {
            // just wanted an empty space, return
            if (!search_exact) {
                *found_index = new_idx;
                return true;
            }
            // if landed on totally unused index, the entry we're looking for doesn't exist 
            if (!(entry->state & TOMB)) {
                return false;
            }
        }
        // check if occupied space has same key. If so, the index can be returned
        if (map->key_compare_func(key, entry->key)) {
            *found_index = new_idx;
            return true;
        }
        j+=1;
        count+=1;
    }
    return false;
}

/* Increases size of hashmap if load factor exceeds a certain value*/
int resize_table(JHASHMAP* map, bool grow) {
    size_t i, old_capacity, new_capacity;
    JHASHMAP_ENTRY* old_vector;
    
    old_capacity = map->capacity;
    if (grow) {
        new_capacity = old_capacity * 2;
         // check for overflow
         if (new_capacity / 2 != old_capacity) {
            return -1;
        }
    }
    else {
        new_capacity = old_capacity / 2;
    }

    old_vector = map->vector;

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
        if (old_vector[i].state == 1) {
            void* old_key = old_vector[i].key;
            void* old_value = old_vector[i].value;

            size_t index = map->hash_func(old_key) % map->capacity;

            // this space is already occupied and doesn't have the same key. Use quadratic probing to find an empty one
            if (map->vector[index].state & IN_USE) {
                quadratic_probe(map, old_key, index, &index, false);
            }

            map->vector[index].key = old_key;
            map->vector[index].value = old_value;
            map->vector[index].state = IN_USE;
            map->occupied++;
        }
    }

    /* get rid of old vector*/
    free(old_vector);
    old_vector = NULL;
    
    return 0;
}


/***************************************/