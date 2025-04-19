#include <JVEC.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
 * initialize a new JVEC. Returns a reference to the heap allocated vector.
 */
JVEC* JVEC_new() {
    JVEC* vector = (JVEC*) malloc(sizeof(JVEC));

    if (!vector) {
        perror("Failed to allocate new vector");
        return 1;
    }

    // start off with INITIAL_CAP elements
    void** array = (void**)malloc(sizeof(void*) * INITIAL_CAP);
    // allocation fails
    if (!array) {
        perror("Failed to allocate array memory");
        return 1;
    }

    // set new array to head
    vector->head = array;

    // no items currently in vector
    vector->length = 0;

    // starting capacity is 10
    vector->capacity = INITIAL_CAP;

    return vector;
}


/*
 * append a pointer to a heap allocated variable to the end of the vector
 */
int JVEC_append(JVEC* vector, void* data_ptr) {

    // set next open index to new element
    *(vector->head + vector->length) = data_ptr;

    vector->length++;

    // grow vector if needed
    if (vector->length == vector->capacity) {
        JVEC_grow(vector);
    }

    return 0;
}


/*
 * prepend a pointer to a heap allocated variable to the start of the vector
 */
int JVEC_prepend(JVEC* vector, void* data_ptr) {

    size_t i;
    // the index vector->length is one after the final element
    for (i = vector->length; i > 0; i--) {
        // move previous element forward to current index
        *(vector->head + i) = *(vector->head + i-1);
    }

    *vector->head = data_ptr;

    vector->length++;

    if (vector->length == vector->capacity) {
        JVEC_grow(vector);
    }

    return 0;
}


/*
 * add a pointer to a heap allocated variable to a valid index of the vector. If
 * an element already exists there, it is moved forward. If not (the first open posisition),
 * it is simply appended like normal.
 */
int JVEC_insert_at(JVEC* vector, void* data_ptr, size_t index) {

    // if the index isn't somewhere in the middle or the first open position
    if (index > vector->length) {
        perror("Index out of bounds\n");
        return 1;
    }

    // move existing elements forward if needed
    size_t i;
    // the index vector->length is one after the final element
    for (i = vector->length; i > index; i--) {
        // move previous element forward to current index
        *(vector->head + i) = *(vector->head + i-1);
    }

    // insert new element at specified index
    *(vector->head+index) = data_ptr;

    vector->length+=1;

    //grow vector if needed
    if (vector->length == vector->capacity) {
        JVEC_grow(vector);
    }

    return 0;
}

void* JVEC_pop(JVEC* vector) {

    if (vector->length == 0) {
        return 1;
    }

    void* item = *(vector->head + vector->length - 1);
    vector->length--;

    if (vector->length == vector->capacity/2) {
        JVEC_shrink(vector);
    }

    return item;
}

void* JVEC_index(JVEC* vector, size_t index) {
    if (index >= vector->length) {
        perror("Index out of bounds\n");
        return 1;
    }

    return *(vector->head + index);
}

int JVEC_grow(JVEC* vector) {
    size_t new_capacity = vector->capacity * 2;


    void** new_head = (void**) realloc(vector->head, new_capacity* sizeof(void*));
    if (!new_head) {
        perror("Failed to resize vector.");
        return 1;
    }

    vector->head = new_head;
    vector->capacity = new_capacity;
    return 0;
}

int JVEC_shrink(JVEC* vector) {
    size_t new_capacity = vector->capacity / 2;
    printf("%ld new cap %ld\n", vector->capacity, new_capacity);

    void ** new_head = (void**) realloc(vector->head, new_capacity * sizeof(void*));
    if (!new_head) {
        perror("Failed to resize vector");
        return 1;
    }

    vector->head = new_head;
    vector->capacity = new_capacity;
    return 0;
}


int main(void) {
    char* str1 = (char*) malloc(sizeof(char) * 6);
    strncpy(str1, "hello", 6);

    char* str2 = (char*) malloc(sizeof(char) * 14);
    strncpy(str2, "Second string", 14);

    char* str3 = (char*) malloc(sizeof(char*) * 20);
    strncpy(str3, "string number three", 20);

    /* JVEC_new */
    JVEC* test1 = JVEC_new();

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

}