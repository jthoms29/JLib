
#include <JHASHMAP.h>


JHASHMAP* JHASHMAP_new(size_t (*hash_func)) {
    JHASHMAP* new_map = (JHASHMAP*) malloc(sizeof(JHASHMAP));

    /* Allocate a new hashmap*/
    if (!new_map) {
        perror("error allocating new hash map");
        return NULL;
    }

    /* allocate the hashmap's vector */
    void** new_vector = (void**) calloc(sizeof(void*), INITIAL_CAPACITY);
    if (!new_vector) {
        perror("error allocating hash map's vector");
        free(new_map);
        return NULL;
    }

    /* set map's default values */
    new_map->capacity = INITIAL_CAPACITY;
    new_map->occupied = 0;
    new_map->vector = new_vector;
    new_map->hash_func = hash_func;

    return new_map;
}

size_t JHASHMAP_quadradic_probe(void** vector, size_t index, size_t capacity) {
    size_t j, new_index;
    long p_j, frac;

    /* offset from starting position */
    j = 1;
    for(;;) {
        frac = (j+1)/2;
        p_j = (int64_t) pow(-1, (j-1)) *  (int64_t) pow(((j+1)/2), 2);
        new_index = ((int64_t) index + p_j) % capacity;

        /* found an empty index. return it */
        if (vector[new_index] == NULL) {
            printf("%ld\n", p_j);
            break;
        }
        j+=1;
    }
    return new_index;
}

size_t JHASHMAP_hash_int32(int32_t* num, size_t vec_length) {
    return *num % vec_length;
}

int8_t JHASHMAP_add(JHASHMAP* map, void* key, void* value) {
    // get index from key
    size_t index = map->hash_func(key, map->capacity);

    // this space is already occupied. Use quadratic probing to find an empty one
    if (map->vector[index] != NULL) {
        index = JHASHMAP_quadradic_probe(map->vector, index, map->capacity);
    }

    map->vector[index] = value;

    return 0;
}

int main(void) {
    JHASHMAP* test = JHASHMAP_new(JHASHMAP_hash_int32);
    printf("int64: %ld, int: %ld\n", sizeof(int64_t*), sizeof(int*));
    int32_t key = 3, val = 5;
    int32_t* keyref = &key, *valref = &val;
    JHASHMAP_add(test, keyref, valref);
    JHASHMAP_add(test, keyref, valref);
    JHASHMAP_add(test, keyref, valref);
    JHASHMAP_add(test, keyref, valref);
    JHASHMAP_add(test, keyref, valref);
    JHASHMAP_add(test, keyref, valref);
    JHASHMAP_add(test, keyref, valref);
    JHASHMAP_add(test, keyref, valref);
    
    return 0;
}
