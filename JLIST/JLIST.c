#include <stdio.h>
#include <stdlib.h>
#include <JLIST.h>
#include <time.h>
#include <string.h>

/*
 * initialize a new JLIST. Returns a reference to the heap allocated list.
 */
JLIST* JLIST_new(void) {

    // allocate a new list
    JLIST* list = (JLIST*) malloc(sizeof(JLIST));
    if (!list) {
        perror("Failed to allocate list");
        return NULL;
    }

    // set default values
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;

    return list;
}


/*
 * append a pointer to a heap allocated variable to the end of the list
 */
int JLIST_append(JLIST* list, void* data_ptr) {
    
    if (data_ptr == NULL) {
        perror("data is NULL");
        return 1;
    }

    JNODE* new_node = (JNODE*) malloc(sizeof(JNODE));

    // set up default values for new node
    new_node->next = NULL;
    new_node->prev = list->tail;
    new_node->item = data_ptr;

    // update list values

    //list not empty
    if (list->tail != NULL) {
        list->tail->next = new_node;
    }

    // empty, make head the new node
    else {
        list->head = new_node;
    }

    // appended, is now tail
    list->tail = new_node;

    list->length ++;

    return 0;
}



int main(void) {
    char* str1 = (char*) malloc(sizeof(char) * 6);
    strncpy(str1, "hello", 6);

    char* str2 = (char*) malloc(sizeof(char) * 14);
    strncpy(str2, "Second string", 14);

    char* str3 = (char*) malloc(sizeof(char*) * 20);
    strncpy(str3, "string number three", 20);

    JNODE* walker;

    /* JLIST_new */
    JLIST* test1 = JLIST_new();

    if (!test1) {
        printf("Failed to allocate vector\n");
    }
    if (test1->length != 0) {
        printf("init: length should be 0");
    }


    /* JLIST_append*/

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

    clock_t t1 = clock();

    for(size_t i = 0; i < 999998; i++) {
        JLIST_append(test1, str2);
    }

    t1 = clock() - t1;
    printf("million appends: %ld\n", t1);

    t1 = clock();
    char* test;
    walker = test1->head;
    for(size_t i = 0; i < test1->length; i++) {
     //   printf("%ld\n", walker);
       test = (char*) walker->item; 
       walker = walker->next;
    }
    t1 = clock() - t1;
    printf("million reads: %ld\n", t1);
    /* JVEC_prepend
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


    /* JVEC_insert_at
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
    */

}