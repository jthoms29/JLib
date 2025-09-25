
#include <JHASHSET.h>

JHASHSET* JHASHSET_new(long (*hash_func) (void* value, size_t set_capacity),
bool (*value_compare_func) (void* value1, void* value2)) {
    /* Allocate a new hashset*/
    JHASHSET* new_set = (JHASHSET*) calloc(1, sizeof(JHASHSET));
    if (!new_set) {
        perror("error allocating new hashset");
        return NULL;
    }

    /* allocate the hashset's vector */
    JHASHSET_ENTRY* new_vector = (JHASHSET_ENTRY*) calloc(INITIAL_CAPACITY, sizeof(JHASHSET_ENTRY));

    if (!new_vector) {
        perror("error allocating hash set's vector");
        free(new_set);
        return NULL;
    }

    new_set->vector = new_vector;
    new_set->hash_func = hash_func;
    new_set->value_compare_func = value_compare_func;
    new_set->capacity = INITIAL_CAPACITY;

    pthread_mutex_init(&new_set->set_tex, NULL);
    pthread_cond_init(&new_set->set_cond, NULL);
    new_set->readers = 0;

    return new_set;
}


long JHASHSET_quadratic_probe(JHASHSET* set, void* value, long index) {

    /* offset from starting position */
    long j = 1;
    /* Number of checks. Will resize if reaches size of hashset*/
    long count = 0;
    /* quadratic output based on offset from starting position*/
    long p_j;

    size_t new_index;
    /* check alternating offsets of perfect squares from the found index until an empty space is found*/
    for(;;) {
        /* the fraction in the second power is int division*/
        p_j = pow(-1, (j-1)) *  pow(((j+1)/2), 2);
        new_index = ((long) index + p_j) % set->capacity;

        /* found an empty index. return it */
        if (set->vector[new_index].in_use ==  false) {
            break;
        }
        /* check if occupied space has same value. If so nothing should be done*/
        if (set->value_compare_func(value, set->vector[new_index].value)) {
            return -2;
        }

        j+=1;
        count+=1;
        /* Free index not found */
        if (count == set->capacity) {
            return -1;
        }
    }
    return new_index;
}


/**
 * Adds specified value to the hashset using the specified value. If a value with an
 * identical value is already in the set, it is replaced.
 */
int JHASHSET_add(JHASHSET* set, void* value) {
    double load_factor;
    long index;

    if (set == NULL) {
        printf("JHASHSET_add: set is NULL.\n");
        return 1;
    }
    if (value == NULL) {
        printf("JHASHSET_add: value is NULL.\n");
        return 1;
    }

    pthread_mutex_lock(&set->set_tex);
    /* If there are any threads currently reading this hashset, no modifications may be done.
       this thread must wait. */
    if (set->readers) {
        pthread_cond_wait(&set->set_cond, &set->set_tex);
    }

    /* Loop here so index retrieval can be re-attempted if hashset needs to be resized. */
    for(;;) {
        /* get index from value */
        index = set->hash_func(value, set->capacity);

        // this space is already occupied and doesn't have the same value. Use quadratic probing to find an empty one
        if (set->vector[index].in_use && !set->value_compare_func(value, set->vector[index].value)) {

            index = JHASHSET_quadratic_probe(set, value, index);
            /* If the above function returns -1, a different index wasn't able to be found.
            The size of the vector must be increased*/
            if (index == -1) {
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

    if (index != -2) {
        /* Add value with corresponding value to set */
        set->vector[index].value = value;
        set->vector[index].in_use = true;
        set->occupied++;

        /* If the load factor exceeds .75, increase the size of the table*/
        load_factor = (double) set->occupied / (double) set->capacity;

        if (load_factor > 0.75) {
            grow_set(set);
        }
    }

    pthread_mutex_unlock(&set->set_tex);
    return 0;
}


int JHASHSET_remove(JHASHSET* set, void* value) {
    if (set == NULL) {
        printf("JHASHSET_get: set is NULL.\n");
        return NULL;
    }
    if (value == NULL) {
        printf("JHASHSET_get: value is NULL.\n");
        return NULL;
    }

    pthread_mutex_lock(&set->set_tex);
    if (set->readers) {
        pthread_cond_wait(&set->set_cond, &set->set_tex);
    }


    int ret;

    /* Get the index from the supplied value*/
    long index = set->hash_func(value, set->capacity);
    /* If the values match, this is the index where the value should be removed*/

    /* Otherwise, need to do quadratic probing*/
    if (!set->value_compare_func(value, set->vector[index].value)) {
        index = JHASHSET_quadratic_probe(set, value, index);
    }

    if (index != -1) {
        set->vector[index].value = NULL;
        set->vector[index].in_use = false;
        set->vector[index].previously_in_use = true;
        ret = 0;
    }
    else {ret = 1;}

    pthread_mutex_unlock(&set->set_tex);
    return ret;
}

bool JHASHSET_has(JHASHSET* set, void* value) {

    if (set == NULL) {
        printf("JHASHSET_has: set is NULL.\n");
        return false;
    }
    if (value == NULL) {
        printf("JHASHSET_has: value address is NULL.\n");
        return false;
    }

    pthread_mutex_lock(&set->set_tex);
    set->readers++;
    pthread_mutex_unlock(&set->set_tex);


    bool ret;

    /* Get index from supplied value*/
    long index = set->hash_func(value, set->capacity);

    /* If the values match, return true */
    if (set->vector[index].in_use && set->value_compare_func(value, set->vector[index].value)) {
        ret = true;
    }
    else {
        /* Otherwise, need to do quadratic probing*/
        index = JHASHSET_quadratic_probe(set, value, index);
        
        /* not in set */
        if (index < 0) {
            ret = false;
        }
        else {
            ret = true;
        }
    }

    pthread_mutex_lock(&set->set_tex);
    set->readers--;
    if (set->readers == 0) {
        pthread_cond_signal(&set->set_cond);
    }
    pthread_mutex_unlock(&set->set_tex);
    return ret;
}


int grow_set(JHASHSET* set) {
    long i, old_capacity, new_capacity;
    JHASHSET_ENTRY* old_vector;
   
    old_capacity = set->capacity;
    new_capacity = old_capacity * 2;

    old_vector = set->vector;

    /* check for overflow*/
    if (new_capacity / 2 != old_capacity) {
        printf("Cannot grow set anymore");
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

    /* Rehash all vector elements from the original set into the new one*/
    for(i = 0; i < old_capacity; i++) {
        if (old_vector[i].in_use) {
            void* old_value = old_vector[i].value;

            long index = set->hash_func(old_value, set->capacity);

            // this space is already occupied and doesn't have the same value. Use quadratic probing to find an empty one
            if (set->vector[index].in_use) {

                index = JHASHSET_quadratic_probe(set, old_value, index);
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



/* TYPE SPECIFIC FUNCTIONS ########################### */
/* int */
long JHASHSET_hash_int(void* value, size_t set_capacity) {
    return *((int*)value) % set_capacity;
}

bool JHASHSET_compare_int(void* value1, void* value2) {
    int k1 = *((int*)value1);
    int k2 = *((int*)value2);

    return k1 == k2;
}

