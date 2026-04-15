#include <JVEC.h>
#include <stddef.h>
#include <string.h>


static inline int JVEC_resize(JVEC* vec, enum resize_action act) {
  

    size_t cur_cap = vec->cap;
    size_t cur_pad = vec->pad;

    size_t new_cap = cur_cap;
    size_t new_pad = cur_pad;

    ptrdiff_t start_offset = vec->start - vec->head;

    switch (act) {
        case GROW_RIGHT: new_cap *= 2; break;
        case SHRINK_RIGHT: new_cap /= 2; break;
        case GROW_LEFT: new_pad *= 2; break;
        case SHRINK_LEFT: new_pad /= 2; break;
    }

    void** new_head = (void**) realloc(vec->head, (new_pad + new_cap) * sizeof(void*));
    if (!new_head) {
        perror("Failed to resize vec.");
        return 1;
    }

    void** old_start = new_head + start_offset;
    void** new_start = new_head + new_pad;

    if (new_start != old_start) {
        memmove(new_start, old_start, vec->length*sizeof(void*));
    }
    vec->head = new_head;
    vec->start = new_start;
    vec->cap = new_cap;
    vec->pad = new_pad;

    return 0;
}

/*
 * initialize a new JVEC. Returns a reference to the heap allocated vec.
 */
JVEC* JVEC_new(void (*item_free_func)(void* item)) {
    JVEC* vec = (JVEC*) calloc(1, sizeof(*vec));
    if (!vec) {
        perror("Failed to allocate new vec");
        return NULL;
    }

    // start off with INITIAL_CAP elements, leave INITIAL_PAD space at front
    void** array = calloc(INITIAL_PAD + INITIAL_CAP, sizeof(*array));
    if (!array) {
        perror("Failed to allocate array memory");
        return NULL;
    }

    vec->head = array;
    vec->length = 0;
    vec->cap = INITIAL_CAP;
    vec->pad = INITIAL_PAD;
    // start address. Leave space at front to account for prepends
    vec->start = array + INITIAL_PAD;
    vec->free_func = item_free_func;
    return vec;
}


/*
 * append a pointer to a heap allocated variable to the end of the vec
 */
static inline int JVEC_append_INLINE(JVEC* vec, void* data) {
    if (!vec) {
        return -1;
    }
    // grow vec if needed
    if (vec->length == vec->cap) {
        JVEC_resize(vec, GROW_RIGHT);
    }

    // set next open index to new element
    *(vec->start + vec->length) = data;
    vec->length++;

    return 0;
}
int JVEC_append(JVEC* vec, void* data) {
    return JVEC_append_INLINE(vec, data);
}

/*
 * prepend a pointer to a heap allocated variable to the start of the vec
 */
static inline int JVEC_prepend_INLINE(JVEC* vec, void* data) {
    if (!vec) {
        return -1;
    }
    if (vec->start == vec->head) {
        JVEC_resize(vec, GROW_LEFT);
    }
    vec->start--;
    *(vec->start) = data;

    vec->length++;
    vec->cap++;
    return 0;
}
int JVEC_prepend(JVEC* vec, void* data) {
    return JVEC_prepend_INLINE(vec, data);
}

int JVEC_in_after(JVEC* vec, void* data, size_t idx) {
    size_t len;
    if (!vec || idx > (len = vec->length)) {
        return -1;
    }
    // only concerned about exact index being modified 
    idx++;
    if (!vec || idx > len) {
        return -1;
    }
    if (idx == len) {
        return JVEC_append_INLINE(vec, data);
    }

    size_t cap = vec->cap;
    if (len == cap) {
        JVEC_resize(vec, GROW_RIGHT);
    }

    void** start = vec->start;
    memmove((start+idx)+1, start+idx, (len-idx) * sizeof(void*));

    *(start+idx) = data;
    vec->length++;

    return 0;
}

int JVEC_in_before(JVEC* vec, void* data, size_t idx) {
    size_t len;
    if (!vec || idx > (len = vec->length)) {
        return -1;
    }
    // size_t can't be neg
    if (idx == 0) {
        return JVEC_prepend_INLINE(vec, data);
    }
    // only concerned with exact idx being modified
    idx--;

    return 1;
}

void JVEC_free(JVEC** vec_ptr) {

    free((*vec_ptr)->head);
    free(*vec_ptr);
}


