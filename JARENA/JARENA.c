#include <JARENA.h>


JARENA_NODE* JARENA_alloc_new_node(void) {
    JARENA_NODE* new_node = calloc(1, sizeof(JARENA_NODE));
    if (!new_node) {
        perror("Failed to allocate arena node");
        return NULL;
    }

    byte* new_block = calloc(BLOCK_SIZE, 1);
    if (!new_block) {
        perror("failed to allocate memory block");
        free(new_node);
        return NULL;
    }

    new_node->mem_block = new_block;
    new_node->cur_block_address = new_block;
    new_node->size = BLOCK_SIZE;
    return new_node;
}

JARENA* JARENA_new(void) {
    JARENA* new_arena = calloc(1, sizeof(JARENA));
    if (!new_arena) {
        perror("Failed to allocate new arena");
        return NULL;
    }

    JARENA_NODE* new_node = JARENA_alloc_new_node();
    if (!new_node) {
        free(new_arena);
        return NULL;
    }

    new_arena->head = new_node;
    new_arena->tail = new_node;
    new_arena->allocated_bytes = BLOCK_SIZE;
    return new_arena;
}

byte* JARENA_alloc(JARENA* arena, size_t alloc_amount) {
    /* Special case: TODO Will allocate large block*/
    if (alloc_amount > BLOCK_SIZE) {
        printf("too large");
        return NULL;
    }

    JARENA_NODE* walker = arena->head;
    while (walker) {
        if ( ((walker->mem_block + walker->size) - walker->cur_block_address) >= alloc_amount) {
            break;
        }
    }

    /* No suitable free memory in existing blocks found. Must allocate a new one*/
    if (!walker) {
        JARENA_NODE* new_node = JARENA_alloc_new_node();
        if (!new_node) {
            return NULL;
        }
        arena->tail->next = new_node;
        walker = new_node;
    }

    byte* mem_ret = walker->cur_block_address;
    walker->cur_block_address += alloc_amount;
   
    return mem_ret;
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

int main(void) {
    JARENA* arena = JARENA_new();
    JARENA_NODE* test = (JARENA_NODE*) JARENA_alloc(arena, sizeof(JARENA_NODE));

    JARENA_free(arena);
    arena = NULL;
}