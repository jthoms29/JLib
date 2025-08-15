#include<JLIST.h>
#include<stdio.h>
#include<string.h>

int main(void) {
    char* str1 = (char*) malloc(sizeof(char) * 6);
    strncpy(str1, "hello", 6);

    char* str2 = (char*) malloc(sizeof(char) * 14);
    strncpy(str2, "Second string", 14);

    char* str3 = (char*) malloc(sizeof(char*) * 20);
    strncpy(str3, "string number three", 20);

    JNODE* walker;

    // JLIST_new 
    JLIST* test1 = JLIST_new(free);

    if (!test1) {
        printf("Failed to allocate vector\n");
    }
    if (test1->length != 0) {
        printf("init: length should be 0");
    }


    // JLIST_append

    // append 1
    JLIST_append(test1, str1);
    if (test1->length != 1) {
        printf("append 1: length should be 1");
    }
    printf("append 1: Should be 'hello': %s\n", ((char*) test1->head->item));

    // append 2
    JLIST_append(test1, str2);
    if (test1->length != 2) {
        printf("append 2: length should be 1");
    }
    printf("append 2: Should be 'hello', 'Second string' : ");
    walker = test1->head;
    for (size_t i=0; i < test1->length; i++) {
        printf("%s, ", (char*) walker->item);
        walker = walker->next;
    }
    printf("\n");
    // and backwards
    printf("append 2: Should be 'Second string, 'hello' : ");
    walker = test1->tail;
    for (size_t i=test1->length; i > 0; i--) {
        printf("%s, ", (char*) walker->item);
        walker = walker->prev;
    }
    printf("\n");

    // JLIST_prepend 
    //prepend 1
    JLIST_prepend(test1, str2);
    if (test1->length != 3) {
        printf("prepend 1: length should be 3");
    }
    walker = test1->head;
    printf("prepend 1: Should be 'Second string, 'hello', 'Second string' : ");
    for (size_t i=0; i < test1->length; i++) {
        printf("%s, ", (char*) walker->item);
        walker = walker->next;
    }
    printf("\n");


    // JLIST_insert 
    //insert 1
    JLIST_insert(test1, str3);
    if (test1->length != 4) {
        printf("insert_at 1: length should be 4");
    }
    walker = test1->head;
    printf("insert 1: Should be 'Second string', 'string number three', 'hello', 'Second string' : ");
    for (size_t i=0; i < test1->length; i++) {
        printf("%s, ", (char*) walker->item);
        walker = walker->next;
    }
    printf("\n");

}