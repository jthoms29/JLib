#include <JHASHSET.h>

int main(void) {
    JHASHSET* set = JHASHSET_new(JHASHSET_hash_int, JHASHSET_compare_int);
    int val = 5;
    JHASHSET_add(set, (void*) &val);
   
    int returned_value = (JHASHSET_has(set, (void*) &val));
    printf("%d\n", returned_value);

    int val2 = 6;

    JHASHSET_add(set, (void*) &val2);

    int val3 = 7;

    returned_value = (JHASHSET_has(set, (void*) &val));
    printf("%d\n", returned_value);
    printf("%d\n", JHASHSET_has(set, (void*) &val3));

}