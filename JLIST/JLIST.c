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


    // allocate a chunk of list nodes
    JNODE* nodes = (JNODE*) malloc(sizeof(JNODE) * INITIAL_FREE);
    if (!nodes) {
        perror("Failed to allocate nodes");
        free(list);
        return NULL;
    }

    // link together all of the free nodes. This chain of free
    // nodes will be held in the JLIST struct, this is where it will get new nodes
    size_t i;
    for (i = 0; i < INITIAL_FREE; i++) {
        // if this is the last node, set the next one to null
        if (i == INITIAL_FREE-1) {
            nodes[i].next = -1;
        }
        else {
            nodes[i].next = i+1;
        }
    }
    list->free_index = 0;
    list->free_block_size = INITIAL_FREE;

    // set rest of default values
    list->head = -1;
    list->tail = -1;
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

    // MEM MANAGEMENT STUFF
    size_t new_index = list->free_index;
    JNODE* new_node = list->list_mem + new_index;
    // move free node chain to the next free one
    list->free_index = new_node->next;


    // set up default values for new node
    new_node->next = -1;
    new_node->prev = list->tail;
    new_node->item = data_ptr;

    // update list values

    //list not empty
    if (list->tail != -1) {
        (list->list_mem + list->tail)->next = new_node;
    }

    // empty, make head the new node
    else {
        list->head = new_index;
    }

    // appended, is now tail
    list->tail = new_index;

    list->length ++;

    if (list->length == list->free_block_size) {
        JLIST_grow(list);
    }
    return 0;
}

int JLIST_grow(JLIST* list) {

    size_t new_mem_size = list->free_block_size * 2;
    JNODE* new_mem_array = (JNODE*) realloc(list->list_mem, new_mem_size);
    if (!new_mem_array) {
        perror("Failed to grow memory");
        return 1;
    }
    list->list_mem = new_mem_array;
    list->free_block_size = new_mem_size;
    return 0;
}

int main(void) {
    char* str1 = (char*) malloc(sizeof(char) * 6);
    strncpy(str1, "hello", 6);

    char* str2 = (char*) malloc(sizeof(char) * 14);
    strncpy(str2, "Second string", 14);

    char* str3 = (char*) malloc(sizeof(char*) * 20);
    strncpy(str3, "string number three", 20);

    JNODE* walker = NULL;

    /* JLIST_new */
    JLIST* test1 = JLIST_new();

    if (!test1) {
        printf("Failed to allocate vector\n");
    }
    if (test1->free_block_size != INITIAL_FREE) {
        printf("init: Incorrect capacity\n");
    }
    if (test1->length != 0) {
        printf("init: length should be 0");
    }


    /* JLIST_append*/

    // append 1
    JLIST_append(test1, str1);
    if (test1->free_block_size != INITIAL_FREE) {
        printf("append 1: Incorrect capacity\n");
    }
    if (test1->length != 1) {
        printf("append 1: length should be 1");
    }
    printf("append 1: Should be 'hello': %s\n", ((char*) test1->head->item));

    // append 2
    JLIST_append(test1, str2);
    if (test1->free_block_size != INITIAL_FREE) {
        printf("append 2: Incorrect capacity\n");
    }
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
        printf("append");
        JLIST_append(test1, str2);
    }

    t1 = clock() - t1;
    printf("million appends: %ld\n", t1);

    t1 = clock();
    char* test;
    walker = test1->head;
    for(size_t i = 0; i < test1->length; i++) {
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