#define _POSIX_C_SOURCE 200809L
#include <JHASHMAP.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "../JHELPER.h"




void test_create_map(void) {
    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);
    assert(map != NULL);
    assert(map->occupied == 0);
    assert(map->capacity == INITIAL_CAPACITY);
    JHASHMAP_free(&map);
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

void test_replace(size_t num) {

    char* key = "single key";
    char** vals = gen_unique_strings(num, 128);

    printf("\ntest replace performance\n");

    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);
    long start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        JHASHMAP_add(map, key, vals[i]);
        assert(JHASHMAP_get(map, key) == vals[i]);
        assert(map->occupied == 1);

        if (i != 0) {
            assert(JHASHMAP_get(map, key) != vals[i-1]);
        }
    }

    long tm = cur_ms() - start;

    printf("%ld replacements in %ld ms\n", num, tm);

    JHASHMAP_free(&map);
    free_unique_strings(vals, num);
}

void test_primitive_performance(size_t num) {

    printf("\nprimitive types performance\n");

    JHASHMAP* map = JHASHMAP_new(int_hash, int_compare);

    int* keys = calloc(num, sizeof(*keys));
    int* vals = calloc(num, sizeof(*vals));

    for (size_t i = 0; i < num; i++) {
        keys[i] = (int) i+1; //can't be 0
        vals[i] = rand() % INT_MAX;
    }
    long tm, start = cur_ms();
    // put these in the map
    for (size_t i = 0; i < num; i++) {
        assert(!JHASHMAP_add(map, CAST_INT(keys[i]), CAST_INT(vals[i])));
        // make sure load factor stays low as expected
        assert(map->occupied == i+1);
        assert((double)map->occupied / (double) map->capacity <= 0.5);
    }
    tm = cur_ms() - start;
    printf("%ld insertions in %ld ms\n", num, tm);


    // retrieve them
    start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        char* retrv = JHASHMAP_get(map, CAST_INT(keys[i]));
        assert(retrv != NULL);
        assert(RETRV_INT(retrv) == vals[i]); // make sure proper val returned
    }
    tm = cur_ms() - start;
    printf("%ld retrievals in %ld ms\n", num, tm);

    free(keys);
    free(vals);
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
    test_create_map();
    test_add_and_get(10000);
    test_remove(10000);
    test_collision(10000);
    test_tombstone(10000);
    test_replace(10000);
    test_primitive_performance(10000000);
    fuzz_test(10000, 100000000);

    printf("\nAll JHASHMAP tests passed\n");
    return 0;
}
