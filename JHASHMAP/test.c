#define _POSIX_C_SOURCE 200809L
#include <JHASHMAP.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <time.h>

/* Get current time in milliseconds */
long cur_ms() {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts)) {
        perror("clock_gettime");
    }
    return ts.tv_sec * 1000 + ts.tv_nsec/1000000;
}

void rand_string(char* str, size_t len) {
    const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789!";

    for (size_t i = 0; i < len; i++) {
        int idx = rand() % (sizeof(charset)-1);
        str[i] = charset[idx];
    }
    str[len] = 0;
}

char** gen_unique_strings(size_t num, size_t strlens) {
    char** strs = malloc(num * sizeof(char*));

    for (size_t i = 0; i < num; i++) {
        char rand_part[strlens-2];
        rand_string(rand_part, sizeof(rand_part)-1);

        int len = snprintf(NULL, 0, "%zu_%s", i, rand_part);
        strs[i] = malloc(len+1);
        // make sure unique
        snprintf(strs[i], len + 1, "%zu_%s", i, rand_part);
    }
    return strs;
}

void free_unique_strings(char** strs, size_t num) {
    for (size_t i = 0; i < num; i++) {
        free(strs[i]);
    }
    free(strs);
} 

size_t string_hash(void* key) {
    unsigned char* str = key;
    size_t hash = 1469598103934665603ULL;

    while (*str) {
        hash ^= (size_t)*str++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

size_t force_collide(void* key) {
    return 21;
}

bool string_compare(void* key1, void* key2) {
    return strcmp((char*)key1, (char*)key2) == 0;
}


void test_create_map(void) {
    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);
    assert(map != NULL);
    assert(map->occupied == 0);
    assert(map->capacity == INITIAL_CAPACITY);
    //JHASHMAP_free(&map);
    printf("test_create_map passed\n");
}

void test_free(void) {
    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);
    //assert(JHASHMAP_free(map) == 0);
    printf("test_free passed\n");
}

void test_add_and_get(size_t num) {
    printf("\nadd and get performance:\n");

    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);

    char** keys = gen_unique_strings(num, 128);
    char** vals = gen_unique_strings(num, 128);

    long tm, start = cur_ms();
    // put these in the map
    for (size_t i = 0; i < num; i++) {
        assert(!JHASHMAP_add(map, keys[i], vals[i]));
        // make sure load factor stays low as expected
        assert(map->occupied == i+1);
        assert((double)map->occupied / (double) map->capacity <= 0.5);
    }
    tm = cur_ms() - start;
    printf("%ld insertions in %ld ms\n", num, tm);


    // retrieve them
    start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        char* retrv = JHASHMAP_get(map, keys[i]);
        assert(retrv != NULL);
        assert((char*) retrv == vals[i]); // make sure proper val returned
    }
    tm = cur_ms() - start;
    printf("%ld retrievals in %ld ms\n", num, tm);

    free_unique_strings(keys, num);
    free_unique_strings(vals, num);
    JHASHMAP_free(&map);
}

void test_remove(size_t num) {
    printf("\nremove performance:\n");

    char** keys = gen_unique_strings(num, 128);
    char** vals = gen_unique_strings(num, 128);

    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);
    for (size_t i = 0; i < num; i++) {
        JHASHMAP_add(map, keys[i], vals[i]);
    }
    assert(map->occupied == num);
    
    long tm, start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        assert(JHASHMAP_remove(map, keys[i]));
        assert(map->occupied == num - (i+1));
        assert(map->occupied < INITIAL_CAPACITY || (double)map->occupied / (double) map->capacity >= 0.25);
    }
    tm = cur_ms() - start;
    printf("%ld removals in %ld ms\n", num, tm);

    JHASHMAP_free(&map);
    free_unique_strings(keys, num);
    free_unique_strings(vals, num);

}

void test_collision(size_t num) {
    printf("\ncollision performance:\n");

    JHASHMAP* map = JHASHMAP_new(force_collide, string_compare);

    char** keys = gen_unique_strings(num, 128);
    char** vals = gen_unique_strings(num, 128);

    long tm, start = cur_ms();
    // put these in the map
    for (size_t i = 0; i < num; i++) {
        assert(!JHASHMAP_add(map, keys[i], vals[i]));
        // make sure load factor stays low as expected
        assert(map->occupied == i+1);
        assert((double)map->occupied / (double) map->capacity <= 0.5);
    }
    tm = cur_ms() - start;
    printf("%ld insertions in %ld ms\n", num, tm);


    // retrieve them
    start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        char* retrv = JHASHMAP_get(map, keys[i]);
        assert(retrv != NULL);
        assert((char*) retrv == vals[i]); // make sure proper val returned
    }
    tm = cur_ms() - start;
    printf("%ld retrievals in %ld ms\n", num, tm);

    free_unique_strings(keys, num);
    free_unique_strings(vals, num);
    JHASHMAP_free(&map);
}

void test_tombstone(size_t num) {
    printf("\ntombstone performance:\n");

    JHASHMAP* map = JHASHMAP_new(force_collide, string_compare);

    char** keys = gen_unique_strings(num, 128);
    char** vals = gen_unique_strings(num, 128);

    long tm, start = cur_ms();
    // put these in the map
    for (size_t i = 0; i < num; i++) {
        assert(!JHASHMAP_add(map, keys[i], vals[i]));
        // make sure load factor stays low as expected
        assert(map->occupied == i+1);
        assert((double)map->occupied / (double) map->capacity <= 0.5);
    }
    tm = cur_ms() - start;
    printf("%ld insertions in %ld ms\n", num, tm);


    // delete every second input
    for (size_t i = 0; i < num/2; i++) {
        assert(JHASHMAP_remove(map, keys[i*2]));
        // now attempt getting the element immediately after
        assert(JHASHMAP_get(map, keys[i*2+1]));
    }
    // retreive all inputs that still exist



    tm = cur_ms() - start;
    printf("%ld retrievals in %ld ms\n", num, tm);

    free_unique_strings(keys, num);
    free_unique_strings(vals, num);
    JHASHMAP_free(&map);
}

void fuzz_test(size_t num, size_t iter) {
    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);

    char** keys = gen_unique_strings(num, 64);
    char** vals = gen_unique_strings(num, 64);

    printf("\nfuzz test performance\n");
    size_t insertions = 0;
    size_t deletions = 0;
    size_t gets = 0;

    long start = cur_ms();

    for (size_t i = 0; i < iter; i++) {
        int op = rand() % 3;
        int k = rand() % num;

        char* key = keys[k];
        char* val = vals[k];
        char* ret;
        switch (op) {
            case 0:
            JHASHMAP_add(map, key, val);
            insertions++;
            break;

            case 1:
            if ((ret = JHASHMAP_remove(map, key))) {
                assert(ret == val);
            }
            deletions++;
            break;

            case 2:
            if ((ret =JHASHMAP_get(map, key))) {
                assert(ret == val);
            };
            gets++;
            break;
        }
    } 
    long tm = cur_ms() - start;

    printf("%ld insertions, %ld deletions, %ld gets in %ld ms\n", insertions, deletions, gets, tm);
    printf("%ld occupied, %ld capacity\n", map->occupied, map->capacity);
}


int main(void) {
    //test_create_map();
    test_add_and_get(10000);
    test_remove(10000);
    test_collision(10000);
    test_tombstone(10000);
    fuzz_test(10000, 10000000);

    printf("\nAll JHASHMAP tests passed\n");
    return 0;
}
