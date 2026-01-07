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

    /// address of element in node
    void* item;

    /// pointer to previous node
    struct JNODE* prev;

    /// pointer to next node
    struct JNODE* next;
} JNODE;


/**
 * The list data structure
 */
typedef struct JLIST {
    /// pointer to the first node in the list
    JNODE* head;

    /// index of the last node in the list
    JNODE* tail;

    /// cursor, the current element in the list
    JNODE* cur;

    /// the current length of the list 
    size_t length;

    /// Function that is passed to data structure upon initialization; compares two elements held by it
    int (*comparator)(void* e1, void* e2);

    /// Function that is passed to data structure upon initialization; frees the datatype held by the vector
    void (*free_func)(void* item);

} JLIST;

/**
 * Initialize a new JLIST. Need to supply a function to compare items within it and to free the items. If items aren't heap allocated, item_free_func should be NULL
 @param[in] comparator
 @param[in] item_free_func
 \return Pointer to a new list
 */
JLIST* JLIST_new( int (*comparator)(void* e1, void* e2), void (*item_free_func)(void* item) );

/**
 * Append a pointer to a variable to the end of the list
 * @param[in, out] list
 * @param[in] data
 * \return 0 on success, anything else on failure
 */
int JLIST_append(JLIST *list, void* data);


/**
 * Prepend a pointer to a variable to the end of the list
 * @param[in, out] list
 * @param[in] data
 * \return 0 on success, anything else on failure
 */
int JLIST_prepend(JLIST* list, void* data);

/**
 * Insert a pointer to a variable after the item the cursor is currently on
 * @param[in, out] list
 * @param[in] data
 * \return 0 on success, anything else on failure
 */
int JLIST_insert(JLIST* list, void* data);

/**
 * Remove the last item in the list.
 * @param[in, out] list
 * \return The last item in the list
 */
void* JLIST_pop(JLIST* list);


/**
 * Move cursor to the first item in the list 
 * @param[in, out] list
 * \return 0 on success, anything else on failure
 */
int JLIST_first(JLIST* list);

/**
 * Move cursor to the last item in the list 
 * @param[in, out] list
 * \return 0 on success, anything else on failure
 */
int JLIST_last(JLIST* list);

/**
 * Move cursor to the next item in the list 
 * @param[in, out] list
 * \return 0 on success, anything else on failure
 */
int JLIST_next(JLIST* list);

/**
 * Move cursor to the previous item in the list 
 * @param[in, out] list
 * \return 0 on success, anything else on failure
 */
int JLIST_prev(JLIST* list);

/**
 * Get item at the current cursor location 
 * @param[in] list
 * \return The item. NULL on failure
 */
void* JLIST_get(JLIST* list);


/**
 * Get the current number of items in the list
 * @param[in] list
 * \return Length of the list
 */
size_t JLIST_len(JLIST* list);


/**
 * Sort the list, uses comparator function passed in on creation
 * @param[in, out] list
 */
void JLIST_sort(JLIST* list);

/**
 * Free each item in the list. Uses the free function passed in on creation
 */
void JLIST_free(JLIST* list);



#endif