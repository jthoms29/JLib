#include <JVEC.h>

int main(void) {
    char* str1 = (char*) malloc(sizeof(char) * 6);
    strncpy(str1, "hello", 6);

    char* str2 = (char*) malloc(sizeof(char) * 14);
    strncpy(str2, "Second string", 14);

    char* str3 = (char*) malloc(sizeof(char*) * 20);
    strncpy(str3, "string number three", 20);

    /* JVEC_new */
    JVEC* test1 = JVEC_new(free);

    if (!test1) {
        printf("Failed to allocate vector\n");
    }
    if (test1->capacity != INITIAL_CAP) {
        printf("init: Incorrect capacity\n");
    }
    if (test1->length != 0) {
        printf("init: length should be 0");
    }


    /* JVEC_append*/

    // append 1
    JVEC_append(test1, str1);
    if (test1->capacity != INITIAL_CAP) {
        printf("append 1: Incorrect capacity\n");
    }
    if (test1->length != 1) {
        printf("append 1: length should be 1");
    }
    printf("append 1: Should be 'hello': %s\n", ((char*) test1->head[0]));

    // append 2
    JVEC_append(test1, str2);
    if (test1->capacity != INITIAL_CAP) {
        printf("append 2: Incorrect capacity\n");
    }
    if (test1->length != 2) {
        printf("append 2: length should be 1");
    }
    printf("append 2: Should be 'hello', 'Second string' : ");
    for (size_t i=0; i < test1->length; i++) {
        printf("%s, ", (char*) test1->head[i]);
    }
    printf("\n");

    /* JVEC_prepend */
    //prepend 1
    JVEC_prepend(test1, str2);
    if (test1->capacity != INITIAL_CAP) {
        printf("prepend 1: Incorrect capacity\n");
    }
    if (test1->length != 3) {
        printf("prepend 1: length should be 3");
    }
    printf("prepend 1: Should be 'Second string, 'hello', 'Second string' : ");
    for (size_t i=0; i < test1->length; i++) {
        printf("%s, ", (char*) test1->head[i]);
    }
    printf("\n");


    /* JVEC_insert_at */
    //insert_at 1
    JVEC_insert_at(test1, str3, 1);
    if (test1->capacity != INITIAL_CAP) {
        printf("insert_at 1: Incorrect capacity\n");
    }
    if (test1->length != 4) {
        printf("insert_at 1: length should be 4");
    }
    printf("insert_at 1: Should be 'Second string', 'string number three', 'hello', 'Second string' : ");
    for (size_t i=0; i < test1->length; i++) {
        printf("%s, ", (char*) test1->head[i]);
    }
    printf("\n");

    /* JVEC_get_at*/
    //get_at 1
    char* gotten_at = JVEC_get_at(test1, 2);
    printf("Should be 'hello': %s\n", gotten_at);


    /* JVEC_pop*/








    clock_t t1 = clock();
    for (size_t i = 0; i < 1000000; i++) {
        JVEC_append(test1, str3);
    }
    t1 = clock() - t1;

    printf("million appends: %ld\n", t1);

    t1 = clock();
    char* testspeed;
    for (size_t i = 0; i < test1->length; i++) {
        testspeed = (char*) test1->head[i];
    }

    t1 = clock() - t1;

    printf("million reads: %ld\n", t1);
}