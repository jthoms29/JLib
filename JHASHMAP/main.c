#include <JHASHMAP.h>

int main(void) {
    JHASHMAP* map = JHASHMAP_new(JHASHMAP_hash_int, JHASHMAP_compare_int);
    //int key = 3, val = 5;
    //int* keyref = &key, *valref = &val;
    //JHASHMAP_add(test, (void*) keyref, (void*) valref);
   
    //void* returned_value = (JHASHMAP_get(test, keyref));
    //printf("%p, %p", valref, returned_value);

    //printf("hello %d", JHASHMAP_has(test, keyref));



    int nums[] = {2, 7, 11, 15};
    int numsSize = 4, target = 9;
    int i, check;
    int* indexptr, *checkptr;
    for (i = 0; i < numsSize; i++) {
        indexptr = (int*) malloc(sizeof(int));
        *indexptr = i;
        JHASHMAP_add(map, (void*) (nums+i), (void*) indexptr);
    }

    for (i = 0; i < numsSize; i++) {
        check = target - nums[i];
        checkptr = &check;
        if (JHASHMAP_has(map, (void*) checkptr)) {
            check=1;
        }
    }
    return 0;
}