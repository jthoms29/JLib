#include <stdio.h>
#include <stdlib.h>
#include <JLIST.h>
#include <string.h>
/*
 * initialize a new JLIST. Returns a reference to the heap allocated list.
 */
JLIST* JLIST_new(void) {

    // allocate a new list
    JLIST* list = (JLIST*) calloc(1,sizeof(JLIST));
    if (!list) {
        perror("Failed to allocate list");
        return NULL;
    }

    // set default values
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    pthread_mutex_init(&list->list_tex, NULL);

    return list;
}


/*
 * append a pointer to a heap allocated variable to the end of the list
 */
int JLIST_append(JLIST* list, void* data_ptr) {

    if (!list) {
        printf("Error: List is null\n");
        return 1;
    }
    if (data_ptr == NULL) {
        perror("data is NULL");
        return 1;
    }

    pthread_mutex_lock(&list->list_tex);
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
        // If the list was previously empty, set the cursor to the new item.
        list->cur = new_node;
    }

    // appended, is now tail
    list->tail = new_node;

    list->length ++;

    pthread_mutex_unlock(&list->list_tex);
    return 0;
}

/*
 * Insert a pointer to a heap allocated variable at the start of the list.
 */
int JLIST_prepend(JLIST* list, void* data_ptr) {
    if (!list) {
        printf("Error: List is null\n");
        return 1;
    }
    if (!data_ptr) {
        printf("Error: data pointer is null\n");
        return 1;
    }

    pthread_mutex_lock(&list->list_tex);

    // allocate a new node
    JNODE* new_node = (JNODE*) malloc(sizeof(JNODE));
    new_node->item = data_ptr;

    new_node->next = list->head;
    new_node->prev = NULL;

    if (list->head) {
        list->head->prev = new_node;
    }
    /* if the list is empty, this will also be the tail */
    else {
        list->tail = new_node;
        list->cur = new_node;
    }

    list->head = new_node;

    list->length++;
    pthread_mutex_unlock(&list->list_tex);
    return 0;

}

/*
 * Insert a pointer to a heap allocated variable after the cursor's current location.
 * The cursor is moved to this location.
 */
int JLIST_insert(JLIST* list, void* data_ptr) {
    if (!list) {
        printf("Error: List is null\n");
        return 1;
    }
    if (!data_ptr) {
        printf("Error: data pointer is null\n");
        return 1;
    }

    pthread_mutex_lock(&list->list_tex);
    JNODE* cursor = list->cur;

    // allocate a new node
    JNODE* new_node = (JNODE*) malloc(sizeof(JNODE));
    new_node->item = data_ptr;

    // if cursor is null, the list is empty
    if (!cursor) {
        new_node->next = NULL;
        new_node->prev = NULL;
        list->head = new_node;
        list->tail = new_node;
    }

    /* else, insert it in front of the new node. Update all references*/
    else {
        new_node->prev = cursor;
        new_node->next = cursor->next;
        cursor->next = new_node;
        if (new_node->next) {
            new_node->next->prev = new_node;
        }
        // if the new node doesn't have a next node, it's now the tail.
        else {
            list->tail = new_node;
        }
    }

    list->cur = new_node;
    list->length++;
    pthread_mutex_unlock(&list->list_tex);
    return 0;
}

/* Remove the last item in the list and return it.*/
void* JLIST_pop(JLIST* list) {
    if (!list) {
        printf("Error: List is null\n");
        return NULL;
    }

    pthread_mutex_lock(&list->list_tex);
    /* List is empty, return nothing*/
    if (!list->tail) {
        return NULL;
    }

    JNODE* old_tail = list->tail;
    void* item = old_tail->item;

    list->tail = old_tail->prev;
    if (list->tail) {
        list->tail->next = NULL;
    }

    free(old_tail);
    old_tail = NULL;
    pthread_mutex_unlock(&list->list_tex);
    return item;
}

/* CURSOR FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

/*
 * Move to first item in the list
 */
int JLIST_first(JLIST* list) {
    if (!list) {
        printf("Error: List is null\n");
        return 1;
    }

    pthread_mutex_lock(&list->list_tex);
    list->cur = list->head;
    pthread_mutex_unlock(&list->list_tex);
    return 0;
}

/*
 * Move to the last item in the list
 */
int JLIST_last(JLIST* list) {
    if (!list) {
        printf("Error: List is null\n");
        return 1;
    }
    pthread_mutex_lock(&list->list_tex);
    list->cur = list->tail;
    pthread_mutex_unlock(&list->list_tex);
    return 0;
}

/*
 * Move to the next item in the list
 */
int JLIST_next(JLIST* list) {
    if (!list) {
        printf("Error: List is null\n");
        return 1;
    }
    pthread_mutex_lock(&list->list_tex);
    /* If the list is empty, or if at the last item, do nothing. */
    if (!list->cur || !list->cur->next) {
        return 0;
    }

    list->cur = list->cur->next;
    pthread_mutex_unlock(&list->list_tex);
    return 0;
}

/* Move to the previous item in the list */
int JLIST_prev(JLIST* list) {
    if (!list) {
        printf("Error: List is null\n");
        return 1;
    }
    pthread_mutex_lock(&list->list_tex);

    // move cursor back if previous item exists
    if (list->cur && list->cur->prev) {
        list->cur = list->cur->prev;
    }
    pthread_mutex_unlock(&list->list_tex);
    return 0;
}

/* Return the item at the current list location. */
void* JLIST_get(JLIST* list) {
    if (!list) {
        printf("Error: List is null\n");
        return NULL;
    }
    pthread_mutex_lock(&list->list_tex);
    /* If list is empty, do nothing*/
    if (!list->cur) {
        pthread_mutex_unlock(&list->list_tex);
        return NULL;
    }

    pthread_mutex_unlock(&list->list_tex);
    return list->cur->item;
}

int main(void) {
    char* str1 = (char*) malloc(sizeof(char) * 6);
    strncpy(str1, "hello", 6);

    char* str2 = (char*) malloc(sizeof(char) * 14);
    strncpy(str2, "Second string", 14);

    char* str3 = (char*) malloc(sizeof(char*) * 20);
    strncpy(str3, "string number three", 20);

    JNODE* walker;

    // JLIST_new 
    JLIST* test1 = JLIST_new();

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
