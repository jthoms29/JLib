
#include <JHASHMAP.h>
#include <pthread.h>

JHASHMAP* JHASHMAP_new(long (*hash_func) (void* key, size_t map_capacity),
bool (*key_compare_func) (void* key1, void* key2)) {
    /* Allocate a new hashmap*/
    JHASHMAP* new_map = (JHASHMAP*) malloc(sizeof(JHASHMAP));
    if (!new_map) {
        perror("error allocating new hash map");
        return NULL;
    }

    /* allocate the hashmap's vector */
    JHASHMAP_ENTRY* new_vector = (JHASHMAP_ENTRY*) calloc(sizeof(JHASHMAP_ENTRY), INITIAL_CAPACITY);

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

long JHASHMAP_quadradic_probe_insert(JHASHMAP* map, void* key, long index) {

    long j, new_index, p_j;
    size_t count;
    /* offset from starting position */
    j = 1;
    count = 0;
    /* check alternating offsets of perfect squares from the found index until an empty space is found*/
    for(;;) {
        /* the fraction in the second power is int division*/
        p_j = pow(-1, (j-1)) *  pow(((j+1)/2), 2);
        new_index = ((int64_t) index + p_j) % map->capacity;

        /* found an empty index. return it */
        if (map->vector[new_index].in_use ==  false) {
            /*printf("%d\n",p_j);*/
            break;
        }
        /* check if occupied space has same key. If so, it can't be entered*/
        if (map->key_compare_func(key, map->vector[new_index].key)) {
            return -2;
        }

        j+=1;
        count+=1;
        /* Free index not found */
        if (count == map->capacity) {
            printf("yep");
            return -1;
        }
    }
    return new_index;
}

long JHASHMAP_quadradic_probe_get(JHASHMAP* map, void* key, long index) {
    long j, new_index, p_j;
    size_t count;

    /* offset from starting position */
    j = 1;
    count = 0;
    /* check alternating offsets of perfect squares from the found index until an empty space is found*/
    for(;;) {
        /* the fraction in the second power is int division*/
        p_j = pow(-1, (j-1)) *  pow(((j+1)/2), 2);
        new_index = ((int64_t) index + p_j) % map->capacity;

        /* check if the index's key is the same as ours. If so, return this index */
        if (map->vector[new_index].in_use && map->key_compare_func(key, map->vector[new_index].key)) {
            //printf("%d\n",p_j);
            break;
        }
        /* If the current isn't either in use or previously in use, we can't continue searching - the key isn't in the map*/
        if (!map->vector[new_index].in_use && !map->vector[new_index].previously_in_use) {
            return -2;
        }

        j+=1;
        count+=1;
        /* Desired key not found */
        if (count == map->capacity) {
            return -1;
        }
    }
    return new_index;
}


int JHASHMAP_add(JHASHMAP* map, void* key, void* value) {
    double load_factor;
    long index;

    pthread_mutex_lock(&map->map_tex);

    /* If there are any threads currently reading this hashmap, no modifications may be done.
       this thread must wait. */
    if (map->readers) {
        pthread_cond_wait(&map->map_cond, &map->map_tex);
    }

    /* get index from key */
    retry:
    index = map->hash_func(key, map->capacity);

    // this space is already occupied. Use quadratic probing to find an empty one
    if (map->vector[index].in_use) {

        if (map->key_compare_func(key, map->vector[index].key)) {
            printf("cannot add repeat key to map\n");
            return 1;
        }
        index = JHASHMAP_quadradic_probe_insert(map, key, index);
        //printf("%ld\n", index);
        /* If the above function returns -1, a different index wasn't able to be found.
           The size of the vector must be increased*/
        if (index == -1) {
            grow_table(map);
            goto retry;
        }
        /* Key already in map*/
        if (index == -2) {
            printf("cannot add repeat key to map\n");
            return 1;
        }
    }

    map->vector[index].key = key;
    map->vector[index].value = value;
    map->vector[index].in_use = true;
    map->occupied++;

    /* If the load factor exceeds .75, increase the size of the table*/
    load_factor = (double) map->occupied / (double) map->capacity;

    if (load_factor > 0.75) {
        grow_table(map);
    }

    pthread_mutex_unlock(&map->map_tex);
    return 0;
}


void* JHASHMAP_get(JHASHMAP* map, void* key) {
    long index;
    index = map->hash_func(key, map->capacity);

    /* If the keys match, return the item at this index*/
    if (map->key_compare_func(key, map->vector[index].key)) {
        return map->vector[index].value;
    }

    /* Otherwise, need to do quadratic probing*/
    index = JHASHMAP_quadradic_probe_get(map, key, index);
    
    /* not in map */
    if (index < 0) {
        return NULL;
    }
    return map->vector[index].value;
}

bool JHASHMAP_has(JHASHMAP* map, void* key) {
    long index;
    index = map->hash_func(key, map->capacity);

    /* If the keys match, return true */
    if (map->vector[index].in_use && map->key_compare_func(key, map->vector[index].key)) {
        return true;
    }

    /* Otherwise, need to do quadratic probing*/
    index = JHASHMAP_quadradic_probe_get(map, key, index);
    
    /* not in map */
    if (index < 0) {
        return false;
    }
    return true;
}


int8_t grow_table(JHASHMAP* map) {
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

    JHASHMAP_ENTRY* new_vector = (JHASHMAP_ENTRY*) calloc(sizeof(JHASHMAP_ENTRY), new_capacity);
    if (!new_vector) {
        perror("could not allocate resized hashmap");
        return 1;
    }

    map->capacity = new_capacity;
    map->occupied = 0;
    map->vector = new_vector;

    /* Rehash all vector from the original table into the new one*/
    for(i = 0; i < old_capacity; i++) {
        if (old_vector[i].in_use) {
            JHASHMAP_add(map, old_vector[i].key, old_vector[i].value);
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

