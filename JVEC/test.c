#include <JVEC.h>
#include <assert.h>
#include "../JHELPER.h"

void test_append_retrieve(size_t num) {
    char** vals = gen_unique_strings(num, 32);
    JVEC* vec = JVEC_new(NULL);

    printf("\nAppend performance:\n");
    long tm, start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        JVEC_append(vec, vals[i]);
        assert(vec->length == i+1);
    }
    tm = cur_ms() - start;
    printf("%ld appends in %ld ms\n", num, tm);

    start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        char* ret = JVEC_get(vec, i);
        assert(ret == vals[i]);
    }
    tm = cur_ms() - start;
    printf("%ld retrievals in %ld ms\n", num, tm);


    JVEC_free(&vec);
    free_unique_strings(vals, num);
}

void test_prepend_retrieve(size_t num) {

    printf("\nPrepend performance:\n");
    char** vals = gen_unique_strings(num, 32);
    JVEC* vec = JVEC_new(NULL);

    long tm, start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        JVEC_prepend(vec, vals[i]);
        assert(vec->length == i+1);
    }
    tm = cur_ms() - start;
    printf("%ld prepends in %ld ms\n", num, tm);

    start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        char* ret = JVEC_get(vec, i);
        assert(ret == vals[num-(i+1)]);
    }
    tm = cur_ms() - start;
    printf("%ld retrievals in %ld ms\n", num, tm);


    JVEC_free(&vec);
    free_unique_strings(vals, num);
}

void test_in_after(size_t num) {
    printf("\nin_after performance:\n");
    char** vals = gen_unique_strings(num, 32);
    JVEC* vec = JVEC_new(NULL);

    for (size_t i = 0; i < num; i+=2) {
        JVEC_append(vec, vals[i]);
    }
    size_t len = vec->length;

    assert(len == num/2);

    long tm, start = cur_ms();
    // now insert the other items
    for (size_t i = 1; i < num; i+=2) {
        assert(!JVEC_in_after(vec, vals[i], i-1));
        assert(vec->length == len+1);
        len++;
    }
    tm = cur_ms() - start;
    printf("%ld in_afters in %ld ms\n", num, tm);

    start = cur_ms();
    for (size_t i = 0; i < num; i++) {
        char* ret = JVEC_get(vec, i);
        assert(ret == vals[i]);
    }
    tm = cur_ms() - start;
    printf("%ld retrievals in %ld ms\n", num, tm);

    JVEC_free(&vec);
    free_unique_strings(vals, num);
}


int main(void) {

    test_append_retrieve(100000);
    test_prepend_retrieve(100000);
    test_in_after(100000);
}