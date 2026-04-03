#include <JARENA.h>
#include <stdio.h>



/*@
    requires size_of_block > 0;
    assigns \result \from size_of_block;

    ensures \result == \null ||
        (\valid(\result) &&
        \valid(\result->mem_block + (0 .. size_of_block-1)) &&
        \result->cur_block_address == \result->mem_block &&
        \result->size == size_of_block);
    
    ensures \result != \null ==>
        \initialized(\result->mem_block + (0 .. size_of_block-1));
*/
JARENA_NODE* JARENA_alloc_new_node(size_t size_of_block) {
    JARENA_NODE* new_node = calloc(1, sizeof(JARENA_NODE));
    if (!new_node) {
        perror("Failed to allocate arena node");
        return NULL;
    }

    byte* new_block = calloc(size_of_block, 1);
    if (!new_block) {
        perror("failed to allocate memory block");
        free(new_node);
        return NULL;
    }

    new_node->mem_block = new_block;
    new_node->cur_block_address = new_block;
    new_node->size = size_of_block;
    return new_node;
}

/*@
    requires \true;
    assigns \result;
    allocates \result;
    ensures \result == \null ||
        (\valid(\result) &&
        \result->head == \null &&
        \result->tail == \null &&
        \result->allocated_bytes == 0);

 */
JARENA* JARENA_new(void) {
    JARENA* new_arena = calloc(1, sizeof(JARENA));
    if (!new_arena) {
        perror("Failed to allocate new arena");
        return NULL;
    }

    new_arena->head = NULL;
    new_arena->tail = NULL;
    new_arena->allocated_bytes = 0;
    return new_arena;
}


/*@
    requires arena != \null;
    requires alloc_amount > 0;

    assigns \result, arena->allocated_bytes;

    ensures \result == \null ||
        (\valid(\result + (0 .. alloc_amount-1)) &&
         arena->allocated_bytes == \old(arena->allocated_bytes) + alloc_amount);
*/
byte* JARENA_alloc(JARENA* arena, size_t alloc_amount) {

    JARENA_NODE* walker = arena->head;
    while (walker) {
        if ( ((walker->mem_block + walker->size) - walker->cur_block_address) >= alloc_amount) {
            break;
        }
        walker = walker->next;
    }

    /* No suitable free memory in existing blocks found. Must allocate a new one*/
    if (!walker) {
        size_t new_block_size;
        if (alloc_amount > BLOCK_SIZE) {
            new_block_size = alloc_amount;
        }
        else {
            new_block_size = BLOCK_SIZE;
        }
        JARENA_NODE* new_node = JARENA_alloc_new_node(new_block_size);
        if (!new_node) {
            return NULL;
        }

        if (!arena->head) {
            arena->head = new_node;
            arena->tail = new_node;
        }
        else {
            arena->tail->next = new_node;
            arena->tail = new_node;
        }
        walker = new_node;
    }

    byte* mem_ret = walker->cur_block_address;
    walker->cur_block_address += alloc_amount;

    arena->allocated_bytes += alloc_amount;
   
    return mem_ret;
}

byte* JARENA_realloc(JARENA* arena, byte* old_mem, size_t old_alloc_amount, size_t new_alloc_amount) {
    byte* new_mem = JARENA_alloc(arena, new_alloc_amount);
    memmove((void*) new_mem, (void*) old_mem, old_alloc_amount);
    return new_mem;
}

int JARENA_free(JARENA* arena) {
    JARENA_NODE* walker = arena->head;
    JARENA_NODE* next;
    while (walker) {
        next = walker->next;
        free(walker->mem_block);
        free(walker);
        walker = next;
    }

    free(arena);
    return 0;
}
