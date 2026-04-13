
#define _POSIX_C_SOURCE 200809L
#include <JHASHSET.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "../JHELPER.h"




void test_create_set(void) {
    JHASHSET* set = JHASHSET_new(string_hash, string_compare);
    assert(set != NULL);
    assert(set->occupied == 0);
    assert(set->capacity == INITIAL_CAPACITY);
    JHASHSET_free(&set);
    printf("test_create_set passed\n");
}

void test_free(void) {
    JHASHSET* set = JHASHSET_new(string_hash, string_compare);
    //assert(JHASHSET_free(set) == 0);
    printf("test_free passed\n");
}

void test_add_and_get(size_t num) {
    printf("\nadd and has performance:\n");

    JHASHSET* set = JHASHSET_new(string_hash, string_compare);

    char** vals = gen_unique_strings(num, 128);

    long tm, start = cur_ms();
    // put these in the set
    for (size_t i = 0; i < num; i++) {
        assert(!JHASHSET_add(set, vals[i]));
        // make sure load factor stays low as expected
        assert(set->occupied == i+1);
        assert((double)set->occupied / (double) set->capacity <= 0.5);
    }
    tm = cur_ms() - start;
    printf("%ld insertions in %ld ms\n", num, tm);


    // check if has them
    start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        assert(JHASHSET_has(set, vals[i]) == true);
    }
    tm = cur_ms() - start;
    printf("%ld checks in %ld ms\n", num, tm);

    free_unique_strings(vals, num);
    JHASHSET_free(&set);
}

void test_remove(size_t num) {
    printf("\nremove performance:\n");

    char** vals = gen_unique_strings(num, 128);

    JHASHSET* set = JHASHSET_new(string_hash, string_compare);
    for (size_t i = 0; i < num; i++) {
        JHASHSET_add(set, vals[i]);
    }
    assert(set->occupied == num);
   
    char* ret;
    long tm, start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        assert((ret = (char*) JHASHSET_remove(set, vals[i])));
        assert(ret == vals[i]);
        assert(set->occupied == num - (i+1));
        assert(set->occupied < INITIAL_CAPACITY || (double)set->occupied / (double) set->capacity >= 0.25);
    }
    tm = cur_ms() - start;
    printf("%ld removals in %ld ms\n", num, tm);

    JHASHSET_free(&set);
    free_unique_strings(vals, num);

}

void test_collision(size_t num) {
    printf("\ncollision performance:\n");

    JHASHSET* set = JHASHSET_new(force_collide, string_compare);

    char** vals = gen_unique_strings(num, 128);

    long tm, start = cur_ms();
    // put these in the set
    for (size_t i = 0; i < num; i++) {
        assert(!JHASHSET_add(set, vals[i]));
        // make sure load factor stays low as expected
        assert(set->occupied == i+1);
        assert((double)set->occupied / (double) set->capacity <= 0.5);
    }
    tm = cur_ms() - start;
    printf("%ld insertions in %ld ms\n", num, tm);


    // retrieve them
    start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        assert(JHASHSET_has(set, vals[i]));
    }
    tm = cur_ms() - start;
    printf("%ld checks in %ld ms\n", num, tm);

    free_unique_strings(vals, num);
    JHASHSET_free(&set);
}

void test_tombstone(size_t num) {
    printf("\ntombstone performance:\n");

    JHASHSET* set = JHASHSET_new(force_collide, string_compare);

    char** vals = gen_unique_strings(num, 128);

    long tm, start = cur_ms();
    // put these in the set
    for (size_t i = 0; i < num; i++) {
        assert(!JHASHSET_add(set, vals[i]));
        // make sure load factor stays low as expected
        assert(set->occupied == i+1);
        assert((double)set->occupied / (double) set->capacity <= 0.5);
    }
    tm = cur_ms() - start;
    printf("%ld insertions in %ld ms\n", num, tm);


    // delete every second input
    for (size_t i = 0; i < num/2; i++) {
        assert(JHASHSET_remove(set, vals[i*2]));
        assert(JHASHSET_has(set, vals[i*2]) == false);
        // now attempt getting the element immediately after
        assert(JHASHSET_has(set, vals[i*2+1]) == true);
    }
    // retreive all inputs that still exist


    tm = cur_ms() - start;
    printf("%ld retrievals in %ld ms\n", num, tm);

    free_unique_strings(vals, num);
    JHASHSET_free(&set);
}

void test_replace(size_t num) {

    char* val = "single val";

    printf("\ntest replace performance\n");

    JHASHSET* set = JHASHSET_new(string_hash, string_compare);
    long start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        JHASHSET_add(set, val);
        assert(JHASHSET_has(set, val));
        assert(set->occupied == 1);
    }

    long tm = cur_ms() - start;

    printf("%ld replacements in %ld ms\n", num, tm);
    JHASHSET_free(&set);
}

void test_primitive_performance(size_t num) {

    printf("\nprimitive types performance\n");

    JHASHSET* set = JHASHSET_new(int_hash, int_compare);

    int* vals = calloc(num, sizeof(*vals));

    for (size_t i = 0; i < num; i++) {
        vals[i] = (int) i+1; //can't be 0
    }
    long tm, start = cur_ms();
    // put these in the set
    for (size_t i = 0; i < num; i++) {
        assert(!JHASHSET_add(set, CAST_INT(vals[i])));
        // make sure load factor stays low as expected
        assert(set->occupied == i+1);
        assert((double)set->occupied / (double) set->capacity <= 0.5);
    }
    tm = cur_ms() - start;
    printf("%ld insertions in %ld ms\n", num, tm);


    void* retrv;
    // retrieve them
    start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        assert(JHASHSET_has(set, CAST_INT(vals[i])) == true);
        retrv = JHASHSET_remove(set, CAST_INT(vals[i]));
        assert(RETRV_INT(retrv) == vals[i]); // make sure proper val returned
    }
    tm = cur_ms() - start;
    printf("%ld retrievals in %ld ms\n", num, tm);

    free(vals);
    JHASHSET_free(&set);
}

void fuzz_test(size_t num, size_t iter) {
    JHASHSET* set = JHASHSET_new(string_hash, string_compare);

    char** vals = gen_unique_strings(num, 64);

    printf("\nfuzz test performance\n");
    size_t insertions = 0;
    size_t deletions = 0;
    size_t gets = 0;

    long start = cur_ms();

    for (size_t i = 0; i < iter; i++) {
        int op = rand() % 3;
        int k = rand() % num;

        char* val = vals[k];
        char* ret;
        switch (op) {
            case 0:
            JHASHSET_add(set, val);
            insertions++;
            break;

            case 1:
            if ((ret = JHASHSET_remove(set, val))) {
                assert(ret == val);
            }
            deletions++;
            break;

            case 2:
            JHASHSET_has(set, val);
            gets++;
            break;
        }
    } 
    long tm = cur_ms() - start;

    printf("%ld insertions, %ld deletions, %ld gets in %ld ms\n", insertions, deletions, gets, tm);
    printf("%ld occupied, %ld capacity\n", set->occupied, set->capacity);

    free(vals);
    JHASHSET_free(&set);
}


int main(void) {
    test_create_set();
    test_add_and_get(10000);
    test_remove(10000);
    test_collision(10000);
    test_tombstone(10000);
    test_replace(10000);
    test_primitive_performance(10000000);
    fuzz_test(10000, 100000000);

    printf("\nAll JHASHSET tests passed\n");
    return 0;
}
