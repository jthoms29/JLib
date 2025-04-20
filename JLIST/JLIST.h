#ifndef JLIST_H
#define JLIST_H

#include <stdlib.h>

#define INITIAL_FREE 10

typedef size_t JL_ENTRY;
/*
 * Implementation of a doubly linked list.
 */
typedef struct JNODE {
    // address of element in node
    void* item;

    // address of previous node
    JL_ENTRY prev;

    // address of next node
    JL_ENTRY next;
} JNODE;


typedef struct JLIST {
    // index of the first node in list
    JL_ENTRY head;

    // index of the last node in the list
    JL_ENTRY tail;

    // index of the first free block in the list
    JL_ENTRY free_index;

    // the current length of the list 
    size_t length;

    // the amount of free nodes currently allocated
    size_t free_block_size;

    // a pointer to the array used as list memory
    JNODE* list_mem;

} JLIST;

/*
 * initialize a new JLIST. Returns a reference to the heap allocated list.
 */
JLIST* JLIST_new(void);

/*
 * append a pointer to a heap allocated variable to the end of the list
 */
int JLIST_append(JLIST *list, void* data_ptr);


/*
 * prepend a pointer to a heap allocated variable to the start of the list
 */
int JLIST_prepend(JLIST* list, void* data_ptr);




size_t JLIST_len(JLIST* list);

int JLIST_free(JLIST* list);



#endif