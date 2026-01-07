#include <JHASHMAP.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>



long string_hash(void* key, size_t map_capacity) {
    unsigned char* str = (unsigned char*)key;
    unsigned long hash = 5381;

    while (*str) {
        hash = ((hash << 5) + hash) + *str;
        str++;
    }

    return (long)(hash % map_capacity);
}

bool string_compare(void* key1, void* key2) {
    return strcmp((char*)key1, (char*)key2) == 0;
}


void test_create_map(void) {
    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);
    assert(map != NULL);
    assert(map->occupied == 0);
    assert(map->capacity > 0);
    free(map);
    printf("test_create_map passed\n");
}

void test_add_and_get(void) {
    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);

    char* key = "hello";
    int value = 21;

    assert(JHASHMAP_add(map, key, &value) == 0);

    int* result = (int*)JHASHMAP_get(map, key);
    assert(result != NULL);
    assert(*result == 21);

    free(map);
    printf("test_add_and_get passed\n");
}

void test_has(void) {
    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);

    char* key1 = "apple";
    char* key2 = "banana";

    int v1 = 1;
    JHASHMAP_add(map, key1, &v1);

    assert(JHASHMAP_has(map, key1) == true);
    assert(JHASHMAP_has(map, key2) == false);

    free(map);
    printf("test_has passed\n");
}

void test_overwrite_value(void) {
    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);

    char* key = "key";

    int v1 = 10;
    int v2 = 20;

    JHASHMAP_add(map, key, &v1);
    JHASHMAP_add(map, key, &v2);

    int* result = (int*)JHASHMAP_get(map, key);
    assert(result != NULL);
    assert(*result == 20);

    free(map);
    printf("test_overwrite_value passed\n");
}

void test_multiple_entries_and_collisions(void) {
    JHASHMAP* map = JHASHMAP_new(string_hash, string_compare);

    int v1 = 1, v2 = 2, v3 = 3;

    JHASHMAP_add(map, "a", &v1);
    JHASHMAP_add(map, "b", &v2);
    JHASHMAP_add(map, "c", &v3);

    assert(*(int*)JHASHMAP_get(map, "a") == 1);
    assert(*(int*)JHASHMAP_get(map, "b") == 2);
    assert(*(int*)JHASHMAP_get(map, "c") == 3);

    free(map);
    printf("test_multiple_entries_and_collisions passed\n");
}


int main(void) {
    test_create_map();
    test_add_and_get();
    test_has();
    test_overwrite_value();
    test_multiple_entries_and_collisions();

    printf("\nAll JHASHMAP tests passed\n");
    return 0;
}
