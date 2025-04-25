#ifndef JLIST_H
#define JLIST_H

#include <stdlib.h>
#include <stdbool.h>


/*
 * Implementation of a doubly linked list.
 */
typedef struct JNODE {

    // address of element in node
    void* item;

    // pointer to previous node
    struct JNODE* prev;

    // pointer to next node
    struct JNODE* next;
} JNODE;


typedef struct JLIST {
    // index of the first node in list
    JNODE* head;

    // index of the last node in the list
    JNODE* tail;


    // the current length of the list 
    size_t length;

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