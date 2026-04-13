
#include <JHASHMAP.h>
#include <assert.h>
#include <stdint.h>

int resize_table(JHASHMAP* map, int action);
bool find_idx(JHASHMAP* map, void* key, size_t* found_index);
bool find_empty(JHASHMAP* map, void* key, size_t* found_index);

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

    /* get index from key */
    retry:
    if (!find_empty(map, key, &index)) {
        resize_table(map, true);
        goto retry; //sorry big E.W.
    }
    entry = map->vector+index;

    /* Add value with corresponding key to map */
    entry = map->vector+index;
    entry->key = key;
    entry->value = value;
    // only increment num of elements if this isn't replacing a pre-existing element
    if (!(entry->state == IN_USE)) {
        map->occupied++;
        entry->state = IN_USE;
    }

    // resize .5 load factor
    if (map->occupied*2 > map->capacity) {
        resize_table(map, GROW);
    }
    return 0;
}

void* JHASHMAP_remove(JHASHMAP* map, void* key) {
    if (!map || !key) {
        return NULL;
    }
    size_t index;
    JHASHMAP_ENTRY* entry;

    // Key isn't in map
    if (!find_idx(map, key, &index)) { return NULL; }

    entry = map->vector + index;

    entry->state = TOMB;
    map->occupied--;

    void* ret = entry->value;

   
    // resize at .25 load factor
    if (map->capacity > INITIAL_CAPACITY && map->occupied < map->capacity / 4) {
        resize_table(map, SHRINK);
    }

    return ret;
}

void* JHASHMAP_get(JHASHMAP* map, void* key) {
    if (!map || !key) {
        return NULL;
    }

    /* Get the index from the supplied key*/
    size_t index;
    if (!find_idx(map, key ,&index)) {
        // key not in map
        return NULL;
    }
    JHASHMAP_ENTRY* entry = map->vector+index;

    assert(map->key_compare_func(entry->key, key));

    return entry->value;
}


bool JHASHMAP_has(JHASHMAP* map, void* key) {
    if (!map || !key) {
        return false;
    }
    size_t dummy;
    return find_idx(map, key, &dummy);
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
bool find_idx(JHASHMAP* map, void* key, size_t* found_index) {
    JHASHMAP_ENTRY* entry;
   
    size_t mask = (map->capacity-1); // assume power of 2 size
    size_t initial_index = map->hash_func(key) & mask;

    for(size_t i = 0; i < map->capacity; i++) {
        // size_t k = (i + 1)/2; // 0,1,1,2,2,3,3,...
        // size_t offset = k*k & mask;

        // size_t new_idx = (i & 1) 
        //     ? (initial_index + map->capacity - offset) & mask // properly handle negative modulo
        //     : (initial_index + offset) & mask;

        size_t new_idx = (initial_index + i) & mask;
        entry = map->vector+new_idx;
        uint8_t state = entry->state;

        if (state == EMPTY) { return false; }

        int match = (state & IN_USE) && map->key_compare_func(key, entry->key);

        if (match) {
            *found_index = new_idx;
            return true;
        }
    }
    return false;
}

bool find_empty(JHASHMAP* map, void* key, size_t* found_index) {
    // offset from starting position
    JHASHMAP_ENTRY* entry;
   
    size_t mask = (map->capacity-1); // assume power of 2 size
    size_t initial_index = map->hash_func(key) & mask;

    size_t first_tomb = SIZE_MAX;

    for(size_t i = 0; i < map->capacity; i++) {
        // size_t k = (i + 1)/2; // 0,1,1,2,2,3,3,...
        // size_t offset = k*k & mask;

        // // switch between postive and negative square offsets
        // size_t new_idx = (i & 1) 
        //     ? (initial_index + map->capacity - offset) & mask // properly handle negative modulo
        //     : (initial_index + offset) & mask;

        size_t new_idx = (initial_index + i) & mask;

        entry = map->vector+new_idx;
        uint8_t state = entry->state;


        if (state == IN_USE && map->key_compare_func(key, entry->key)) {
            *found_index = new_idx;
            return true;
        }

        if (state == TOMB && first_tomb == SIZE_MAX) {
            first_tomb = new_idx;
            continue;
        }

        // found empty index. If tombstone remembered, use that instead
        if (state == EMPTY) {
            *found_index = (first_tomb == SIZE_MAX)
            ? new_idx
            : first_tomb;
            return true; 
        }

    }

    // if tombstone found, use that. If not, no space
    if (first_tomb == SIZE_MAX) { return false; }
    *found_index = first_tomb;
    return true;
}


/* Increases size of hashmap if load factor exceeds a certain value*/
int resize_table(JHASHMAP* map, int action) {
    size_t i, old_capacity, new_capacity;
    JHASHMAP_ENTRY* old_vector;
    
    old_capacity = map->capacity;
    if (action == GROW) {
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
        if (old_vector[i].state == IN_USE) {
            void* old_key = old_vector[i].key;
            void* old_value = old_vector[i].value;

            JHASHMAP_add(map, old_key, old_value);
        }
    }

    /* get rid of old vector*/
    free(old_vector);
    old_vector = NULL;
    
    return 0;
}


/***************************************/