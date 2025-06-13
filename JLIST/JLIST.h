#ifndef JLIST_H
#define JLIST_H

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
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

    // cursor, the current element in the list
    JNODE* cur;

    // the current length of the list 
    size_t length;

    // thread sync variables. Only one thread will be allowed to modify
    // the list at a time.
    pthread_mutex_t list_tex;

} JLIST;

/*
 * initialize a new JLIST. Returns a reference to the heap allocated list.
 */
JLIST* JLIST_new(void);

/*
 * append a pointer to a heap allocated variable to the end of the list
 */
int JLIST_append(JLIST *list, void* data);


/*
 * prepend a pointer to a heap allocated variable to the start of the list
 */
int JLIST_prepend(JLIST* list, void* data);

/*
* insert a pointer to a heap allocated variable after the current item
*/
int JLIST_insert(JLIST* list, void* data);

/*
* remove the last item in the list
*/
void* JLIST_pop(JLIST* list);

/* CURSOR FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

/* 
 * Move to the first item in the list 
 */
int JLIST_first(JLIST* list);

/* 
 * Move to the last item in the list 
 */
int JLIST_last(JLIST* list);

/* 
 * Move to the next item in the list 
 */
int JLIST_next(JLIST* list);

/* 
 * move to the previous item in the list 
 */
int JLIST_prev(JLIST* list);

/* 
 * get the item at the current location of the cursor 
 */
void* JLIST_get(JLIST* list);


size_t JLIST_len(JLIST* list);

int JLIST_free(JLIST* list);



#endif