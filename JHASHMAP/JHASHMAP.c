
#include <JHASHMAP.h>

JHASHMAP* JHASHMAP_new(long (*hash_func) (void* key, size_t map_capacity),
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
        perror("error allocating hash map's vector");
        free(new_map);
        return NULL;
    }

    new_map->vector = new_vector;
    new_map->hash_func = hash_func;
    new_map->key_compare_func = key_compare_func;
    new_map->capacity = INITIAL_CAPACITY;

    pthread_mutex_init(&new_map->map_tex, NULL);
    pthread_cond_init(&new_map->map_cond, NULL);
    new_map->readers = 0;

    return new_map;
}


long JHASHMAP_quadratic_probe(JHASHMAP* map, void* key, long index) {

    /* offset from starting position */
    long j = 1;
    /* Number of checks. Will resize if reaches size of hashmap*/
    long count = 0;
    /* quadratic output based on offset from starting position*/
    long p_j;

    size_t new_index;
    /* check alternating offsets of perfect squares from the found index until an empty space is found*/
    for(;;) {
        /* the fraction in the second power is int division*/
        p_j = pow(-1, (j-1)) *  pow(((j+1)/2), 2);
        new_index = ((long) index + p_j) % map->capacity;

        /* found an empty index. return it */
        if (map->vector[new_index].in_use ==  false) {
            break;
        }
        /* check if occupied space has same key. If so, the index can be returned*/
        if (map->key_compare_func(key, map->vector[new_index].key)) {
            break;
        }

        j+=1;
        count+=1;
        /* Free index not found */
        if (count == map->capacity) {
            return -1;
        }
    }
    return new_index;
}


/**
 * Adds specified value to the hashmap using the specified key. If a value with an
 * identical key is already in the map, it is replaced.
 */
int JHASHMAP_add(JHASHMAP* map, void* key, void* value) {

    if (map == NULL) {
        printf("JHASHMAP_add: map is NULL.\n");
        return 1;
    }

    if (map == NULL) {
        printf("JHASHMAP_add: map is NULL.\n");
        return 1;
    }
    if (key == NULL) {
        printf("JHASHMAP_add: key is NULL.\n");
        return 1;
    }

    pthread_mutex_lock(&map->map_tex);
    /* If there are any threads currently reading this hashmap, no modifications may be done.
       this thread must wait. */
    if (map->readers) {
        pthread_cond_wait(&map->map_cond, &map->map_tex);
    }

    long index;
    /* Loop here so index retrieval can be re-attempted if hashmap needs to be resized. */
    for(;;) {
        /* get index from key */
        index = map->hash_func(key, map->capacity);

        // this space is already occupied and doesn't have the same key. Use quadratic probing to find an empty one
        if (map->vector[index].in_use && !map->key_compare_func(key, map->vector[index].key)) {

            index = JHASHMAP_quadratic_probe(map, key, index);
            /* If the above function returns -1, a different index wasn't able to be found.
            The size of the vector must be increased*/
            if (index == -1) {
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

    pthread_mutex_unlock(&map->map_tex);
    return 0;
}


void* JHASHMAP_get(JHASHMAP* map, void* key) {
    if (map == NULL) {
        printf("JHASHMAP_get: map is NULL.\n");
        return NULL;
    }
    if (key == NULL) {
        printf("JHASHMAP_get: key is NULL.\n");
        return NULL;
    }

    /* This function only reads from the map, doesn't block access from other readers*/
    pthread_mutex_lock(&map->map_tex);
    map->readers++;
    pthread_mutex_unlock(&map->map_tex);

    /* The value that will be returned */
    void* ret;

    /* Get the index from the supplied key*/
    long index = map->hash_func(key, map->capacity);

    /* If the keys match, return the item at this index*/
    if (map->key_compare_func(key, map->vector[index].key)) {
        ret = map->vector[index].value;
    }

    /* Otherwise, need to do quadratic probing*/
    else {
        index = JHASHMAP_quadratic_probe(map, key, index);
        
        /* specified key is not in map */
        if (index < 0) {
            ret = NULL;
        }
        /* specified key is in map */
        else {
            ret = map->vector[index].value;
        }
    }

    pthread_mutex_lock(&map->map_tex);
    map->readers--;
    /* Wake up a possible waiting writer if there are no more readers*/
    if (map->readers == 0) {
        pthread_cond_signal(&map->map_cond);
    }
    pthread_mutex_unlock(&map->map_tex);
    return ret;
}

bool JHASHMAP_has(JHASHMAP* map, void* key) {

    if (map == NULL) {
        printf("JHASHMAP_has: map is NULL.\n");
        return false;
    }
    if (key == NULL) {
        printf("JHASHMAP_has: key address is NULL.\n");
        return false;
    }

    pthread_mutex_lock(&map->map_tex);
    map->readers++;
    pthread_mutex_unlock(&map->map_tex);


    bool ret;

    /* Get index from supplied key*/
    long index = map->hash_func(key, map->capacity);

    /* If the keys match, return true */
    if (map->vector[index].in_use && map->key_compare_func(key, map->vector[index].key)) {
        ret = true;
    }
    else {
        /* Otherwise, need to do quadratic probing*/
        index = JHASHMAP_quadratic_probe(map, key, index);
        
        /* not in map */
        if (index < 0) {
            ret = false;
        }
        else {
            ret = true;
        }
    }

    pthread_mutex_lock(&map->map_tex);
    map->readers--;
    if (map->readers == 0) {
        pthread_cond_signal(&map->map_cond);
    }
    pthread_mutex_unlock(&map->map_tex);
    return ret;
}


int grow_table(JHASHMAP* map) {
    long i, old_capacity, new_capacity;
    JHASHMAP_ENTRY* old_vector;
   
    old_capacity = map->capacity;
    new_capacity = old_capacity * 2;

    old_vector = map->vector;

    /* check for overflow*/
    if (new_capacity / 2 != old_capacity) {
        printf("Cannot grow table anymore");
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

            long index = map->hash_func(old_key, map->capacity);

            // this space is already occupied and doesn't have the same key. Use quadratic probing to find an empty one
            if (map->vector[index].in_use) {

                index = JHASHMAP_quadratic_probe(map, old_key, index);
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



/* TYPE SPECIFIC FUNCTIONS ########################### */
/* int */
long JHASHMAP_hash_int(void* key, size_t map_capacity) {
    return *((int*)key) % map_capacity;
}

bool JHASHMAP_compare_int(void* key1, void* key2) {
    int k1 = *((int*)key1);
    int k2 = *((int*)key2);

    return k1 == k2;
}

