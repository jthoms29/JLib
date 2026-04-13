#include <JHASHSET.h>
#include <assert.h>
#include <stdint.h>

int resize_set(JHASHSET* set, int action);
bool find_idx(JHASHSET* set, void* val, size_t* found_index);
bool find_empty(JHASHSET* set, void* val, size_t* found_index);

JHASHSET* JHASHSET_new(size_t (*hash_func) (void* val), bool (*val_compare_func) (void* val1, void* val2)) {
    // Allocate a new hashset
    JHASHSET* new_set = (JHASHSET*) calloc(1, sizeof(*new_set));
    if (!new_set) {
        perror("error allocating new hashset");
        return NULL;
    }

    // allocate the hashset's vector
    JHASHSET_ENTRY* new_vector = (JHASHSET_ENTRY*) calloc(INITIAL_CAPACITY, sizeof(*new_vector));

    if (!new_vector) {
        perror("error allocating hashset's vector");
        free(new_set);
        return NULL;
    }

    new_set->vector = new_vector;
    new_set->hash_func = hash_func;
    new_set->val_compare_func = val_compare_func;
    new_set->capacity = INITIAL_CAPACITY;

    return new_set;
}


/*
 * Adds specified value to the hashset using the specified val. If a value with an
 * identical val is already in the set, it is replaced.
 */
int JHASHSET_add(JHASHSET* set, void* val) {
    if (!set || !val) {
        return -1;
    }

    size_t index;
    JHASHSET_ENTRY* entry;

    /* get index from val */
    retry:
    if (!find_empty(set, val, &index)) {
        resize_set(set, true);
        goto retry; //sorry big E.W.
    }
    entry = set->vector+index;

    /* Add value with corresponding val to set */
    entry = set->vector+index;
    entry->val = val;

    // only increment num of elements if this isn't replacing a pre-existing element
    if (!(entry->state == IN_USE)) {
        set->occupied++;
        entry->state = IN_USE;
    }

    // resize .5 load factor
    if (set->occupied*2 > set->capacity) {
        resize_set(set, GROW);
    }
    return 0;
}

void* JHASHSET_remove(JHASHSET* set, void* val) {
    if (!set || !val) {
        return NULL;
    }
    size_t index;
    JHASHSET_ENTRY* entry;

    // val isn't in set
    if (!find_idx(set, val, &index)) { return NULL; }

    entry = set->vector + index;

    entry->state = TOMB;
    set->occupied--;

    void* ret = entry->val;

   
    // resize at .25 load factor
    if (set->capacity > INITIAL_CAPACITY && set->occupied < set->capacity / 4) {
        resize_set(set, SHRINK);
    }
    return ret;
}


bool JHASHSET_has(JHASHSET* set, void* val) {
    if (!set || !val) {
        return false;
    }
    size_t dummy;
    return find_idx(set, val, &dummy);
}

void JHASHSET_free(JHASHSET** set_ptr) {
    free((*set_ptr)->vector);
    free(*set_ptr);
}



/******* PRIV HELPER FUNCTIONS *********/

/*
 Search for the exact val. Uses linear probing
 */
bool find_idx(JHASHSET* set, void* val, size_t* found_index) {
    JHASHSET_ENTRY* entry;
   
    size_t mask = (set->capacity-1); // assume power of 2 size
    size_t initial_index = set->hash_func(val) & mask;

    for(size_t i = 0; i < set->capacity; i++) {

        size_t new_idx = (initial_index + i) & mask;
        entry = set->vector+new_idx;
        uint8_t state = entry->state;

        if (state == EMPTY) { return false; }

        int match = (state & IN_USE) && set->val_compare_func(val, entry->val);

        if (match) {
            *found_index = new_idx;
            return true;
        }
    }
    return false;
}
/*
 Search for either an empty space or the exact val. Uses linear probing.
 */
bool find_empty(JHASHSET* set, void* val, size_t* found_index) {
    // offset from starting position
    JHASHSET_ENTRY* entry;
   
    size_t mask = (set->capacity-1); // assume power of 2 size
    size_t initial_index = set->hash_func(val) & mask;

    size_t first_tomb = SIZE_MAX;

    for(size_t i = 0; i < set->capacity; i++) {

        size_t new_idx = (initial_index + i) & mask;

        entry = set->vector+new_idx;
        uint8_t state = entry->state;


        if (state == IN_USE && set->val_compare_func(val, entry->val)) {
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


/* Resizes or shrinks internal vector by a factor of two */
int resize_set(JHASHSET* set, int action) {
    size_t i, old_capacity, new_capacity;
    JHASHSET_ENTRY* old_vector;
    
    old_capacity = set->capacity;
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

    old_vector = set->vector;

    JHASHSET_ENTRY* new_vector = (JHASHSET_ENTRY*) calloc(new_capacity, sizeof(*new_vector));
    if (!new_vector) {
        perror("could not allocate resized hashset:");
        return 1;
    }

    set->capacity = new_capacity;
    set->occupied = 0;
    set->vector = new_vector;

    /* Rehash all vector elements from the original table into the new one*/
    for(i = 0; i < old_capacity; i++) {
        if (old_vector[i].state == IN_USE) {
            void* old_val = old_vector[i].val;
            JHASHSET_add(set, old_val);
        }
    }
    /* get rid of old vector*/
    free(old_vector);
    old_vector = NULL;
    return 0;
}


/***************************************/