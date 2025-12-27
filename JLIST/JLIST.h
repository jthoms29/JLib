#ifndef JLIST_H
#define JLIST_H


///@file JLIST.h

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

/** 
 * Node that make up the body of the linked list
 */
typedef struct JNODE {

    // address of element in node
    void* item;

    // pointer to previous node
    struct JNODE* prev;

    // pointer to next node
    struct JNODE* next;
} JNODE;


/**
 * The list data structure
 */
typedef struct JLIST {
    /// pointer to the first node in the list
    JNODE* head;

    // index of the last node in the list
    JNODE* tail;

    // cursor, the current element in the list
    JNODE* cur;

    // the current length of the list 
    size_t length;

    /// Function that is passed to data structure upon initialization; frees the datatype held
    /// by the vector
    void (*free_func)(void* item);

    int (*comparator)(void* e1, void* e2);

    // thread sync variables. Only one thread will be allowed to modify
    // the list at a time.
    pthread_mutex_t list_tex;

} JLIST;

/*
 * initialize a new JLIST. Returns a reference to the heap allocated list.
 */
JLIST* JLIST_new( int (*comparator)(void* e1, void* e2), void (*item_free_func)(void* item) );

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


void JLIST_sort(JLIST* list);
void JLIST_free(JLIST* list);



#endif