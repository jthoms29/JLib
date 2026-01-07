#include<JLIST.h>
#include<stdio.h>

#include <stdlib.h>
#include <assert.h>



int int_comparator(void* a, void* b) {
    int ia = *(int*)a;
    int ib = *(int*)b;
    return ia - ib;
}

int* make_int(int value) {
    int* p = malloc(sizeof(int));
    assert(p != NULL);
    *p = value;
    return p;
}


void test_create_and_append(void) {
    JLIST* list = JLIST_new(int_comparator, free);
    assert(list != NULL);
    assert(JLIST_len(list) == 0);

    JLIST_append(list, make_int(10));
    JLIST_append(list, make_int(20));
    JLIST_append(list, make_int(30));

    assert(JLIST_len(list) == 3);

    JLIST_first(list);
    assert(*(int*)JLIST_get(list) == 10);

    JLIST_last(list);
    assert(*(int*)JLIST_get(list) == 30);

    JLIST_free(list);
}

void test_prepend(void) {
    JLIST* list = JLIST_new(int_comparator, free);

    JLIST_append(list, make_int(20));
    JLIST_prepend(list, make_int(10));

    assert(JLIST_len(list) == 2);

    JLIST_first(list);
    assert(*(int*)JLIST_get(list) == 10);

    JLIST_prepend(list, make_int(25));
    JLIST_first(list);
    assert(*(int*)JLIST_get(list) == 25);

    JLIST_free(list);
}

void test_insert(void) {
    JLIST* list = JLIST_new(int_comparator, free);

    JLIST_append(list, make_int(10));
    JLIST_append(list, make_int(30));

    JLIST_first(list);
    JLIST_insert(list, make_int(20));

    assert(JLIST_len(list) == 3);

    JLIST_first(list);
    assert(*(int*)JLIST_get(list) == 10);

    JLIST_next(list);
    assert(*(int*)JLIST_get(list) == 20);

    JLIST_next(list);
    assert(*(int*)JLIST_get(list) == 30);

    JLIST_free(list);
}

void test_navigation(void) {
    JLIST* list = JLIST_new(int_comparator, free);

    JLIST_append(list, make_int(1));
    JLIST_append(list, make_int(2));
    JLIST_append(list, make_int(3));

    JLIST_first(list);
    assert(*(int*)JLIST_get(list) == 1);

    JLIST_next(list);
    assert(*(int*)JLIST_get(list) == 2);

    JLIST_next(list);
    assert(*(int*)JLIST_get(list) == 3);

    JLIST_prev(list);
    assert(*(int*)JLIST_get(list) == 2);

    JLIST_free(list);
}

void test_pop(void) {
    JLIST* list = JLIST_new(int_comparator, free);

    JLIST_append(list, make_int(5));
    JLIST_append(list, make_int(10));

    int* val = (int*)JLIST_pop(list);
    assert(*val == 10);
    free(val);

    assert(JLIST_len(list) == 1);

    val = (int*)JLIST_pop(list);
    assert(*val == 5);
    free(val);

    JLIST_free(list);
}

void test_sort(void) {
    JLIST* list = JLIST_new(int_comparator, free);

    JLIST_append(list, make_int(30));
    JLIST_append(list, make_int(10));
    JLIST_append(list, make_int(20));
    JLIST_append(list, make_int(100));

    JLIST_sort(list);

    //forwards
    JLIST_first(list);
    assert(*(int*)JLIST_get(list) == 10);

    JLIST_next(list);
    assert(*(int*)JLIST_get(list) == 20);

    JLIST_next(list);
    assert(*(int*)JLIST_get(list) == 30);

    JLIST_next(list);
    assert(*(int*)JLIST_get(list) == 100);

    //backwards
    JLIST_last(list);
    assert(*(int*)JLIST_get(list) == 100);

    JLIST_prev(list);
    assert(*(int*)JLIST_get(list) == 30);

    JLIST_prev(list);
    assert(*(int*)JLIST_get(list) == 20);

    JLIST_prev(list);
    assert(*(int*)JLIST_get(list) == 10);


    JLIST_free(list);
}

/* ---------- Main ---------- */

int main(void) {
    printf("Running JLIST tests...\n");

    test_create_and_append();
    test_prepend();
    test_insert();
    test_navigation();
    test_pop();
    test_sort();

    printf("All tests passed\n");
    return 0;
}
