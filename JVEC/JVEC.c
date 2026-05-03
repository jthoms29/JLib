#include <JVEC.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>


static inline int JVEC_resize(JVEC* vec, enum resize_action act) {
    
    // caluculate new capacity and initial pad sizes based on given action 
    // new values initialized to current vals, then modified
    size_t new_cap = vec->cap;
    size_t old_pad = vec->start - vec->head;
    size_t new_pad = old_pad;

    switch (act) {
        case GROW_RIGHT: 
            new_cap *= 2; 
            break;
        case SHRINK_RIGHT:
            assert(new_cap >= INITIAL_CAP); // can't shrink beyond initial size
            new_cap /= 2; 
            break;
        
        // for pad, modify allocation amount, not current size
        case GROW_LEFT: 
            new_pad = vec->pad_alloc * 2; 
            break;
        case SHRINK_LEFT: 
            assert(new_pad >= INITIAL_PAD); // can't shrink beyond initial size
            new_pad = vec->pad_alloc / 2; 
            break;
    }


    void** new_head = (void**) realloc(vec->head, (new_pad + new_cap) * sizeof(void*));
    if (!new_head) {
        perror("Failed to resize vec.");
        return 1;
    }

    void** new_start = new_head + new_pad;
    if (old_pad != new_pad) {
        void** old_start = new_head + old_pad;
        memmove(new_start, old_start, vec->len*sizeof(void*));
    }
    vec->head = new_head;
    vec->start = new_start;
    vec->cap = new_cap;
    vec->pad_alloc = new_pad;

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
    vec->len = 0;
    vec->cap = INITIAL_CAP;
    vec->pad_alloc = INITIAL_PAD;
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
    if (vec->len == vec->cap) {
        JVEC_resize(vec, GROW_RIGHT);
    }

    // set next open index to new element
    *(vec->start + vec->len) = data;
    vec->len++;

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

    vec->len++;
    vec->cap++;
    return 0;
}
int JVEC_prepend(JVEC* vec, void* data) {
    return JVEC_prepend_INLINE(vec, data);
}

int JVEC_in_after(JVEC* vec, void* data, size_t idx) {
    size_t len;
    
    if (!vec || idx >= (len = vec->len)) {
        return -1;
    }
    // only concerned about exact index being modified 
    idx++;
    if (idx == len) {
        return JVEC_append_INLINE(vec, data);
    }

    // want to shift everything before the new item backwards, or everything
    // after it forward - shift whichever has fewer
    void** old_block;
    void** new_block;
    size_t block_len;

    // fewer entries on left side, shift left
    if (idx < len-idx) { // idx known to be leq to len, subtraction is fine
        if (vec->start == vec->head) { 
            JVEC_resize(vec, GROW_LEFT); 
        }
        old_block = vec->start;
        new_block = old_block - 1;
        block_len = idx;
        memmove(new_block, old_block, block_len * sizeof(void*));
        vec->start--;
        vec->cap++;
        vec->start[idx] = data;

    }
    // fewer entries on right side, shift right
    else {
        if (len == vec->cap) { 
            JVEC_resize(vec, GROW_RIGHT); 
        }
        old_block = vec->start + idx;
        new_block = old_block + 1;
        block_len = len - idx;
        memmove(new_block, old_block, block_len * sizeof(void*));
        vec->start[idx] = data;
    }

    vec->len++;

    return 0;
}

int JVEC_in_before(JVEC* vec, void* data, size_t idx) {
    size_t len;
    if (!vec || idx > (len = vec->len)) {
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


