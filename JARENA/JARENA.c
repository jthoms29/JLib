#include <JARENA.h>
#include <string.h>

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

JARENA* JARENA_new(void) {
    JARENA* new_arena = calloc(1, sizeof(JARENA));
    if (!new_arena) {
        perror("Failed to allocate new arena");
        return NULL;
    }

    JARENA_NODE* new_node = JARENA_alloc_new_node(BLOCK_SIZE);
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
        walker = walker->next;
    }

    /* No suitable free memory in existing blocks found. Must allocate a new one*/
    if (!walker) {
        JARENA_NODE* new_node = JARENA_alloc_new_node(BLOCK_SIZE);
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

int main(void) {
    JARENA* arena = JARENA_new();
    JARENA_NODE* test = (JARENA_NODE*) JARENA_alloc(arena, sizeof(JARENA_NODE));
    char* test2 = (char*) JARENA_alloc(arena, sizeof(char)*4096);

    JARENA_free(arena);
    arena = NULL;
}