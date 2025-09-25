#include <JHASHMAP.h>

int main(void) {
    JHASHMAP* map = JHASHMAP_new(JHASHMAP_hash_int, JHASHMAP_compare_int);
    int key = 3, val = 5;
    JHASHMAP_add(map, (void*) &key, (void*) &val);
   
    void* returned_value = (JHASHMAP_get(map, (void*) &key));
    printf("%d, %d\n", key, *((int*)returned_value));

    int val2 = 6;

    JHASHMAP_add(map, (void*) &key, (void*) &val2);


    returned_value = (JHASHMAP_get(map, (void*) &key));
    printf("%d, %d\n", key, *((int*)returned_value));

}