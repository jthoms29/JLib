#include <JHASHMAP.h>

int main(void) {
    JHASHMAP* test = JHASHMAP_new(JHASHMAP_hash_int, JHASHMAP_compare_int);
    int key = 3, val = 5;
    int* keyref = &key, *valref = &val;
    JHASHMAP_add(test, (void*) keyref, (void*) valref);
   
    void* returned_value = (JHASHMAP_get(test, keyref));
    printf("%p, %p", valref, returned_value);
    return 0;
}