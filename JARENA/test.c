#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <JARENA.h>

static void test_arena_new(void) {
    JARENA* arena = JARENA_new();

    assert(arena != NULL);
    assert(arena->head == NULL);
    assert(arena->tail == NULL);
    assert(arena->allocated_bytes == 0);

    JARENA_free(arena);
    printf("test_arena_new passed\n");
}

static void test_single_allocation(void) {
    JARENA* arena = JARENA_new();
    assert(arena);

    byte* mem = JARENA_alloc(arena, 128);
    assert(mem);
    assert(arena->allocated_bytes == 128);
    assert(arena->head);
    assert(arena->tail);
    assert(arena->head == arena->tail);

    /* Write and read test */
    memset(mem, 0xAB, 128);
    for (int i = 0; i < 128; i++) {
        assert(mem[i] == 0xAB);
    }

    JARENA_free(arena);
    printf("test_single_allocation passed \n");
}

static void test_allocations_same_block(void) {
    JARENA* arena = JARENA_new();
    assert(arena);

    byte* a = JARENA_alloc(arena, BLOCK_SIZE/4);
    byte* b = JARENA_alloc(arena, BLOCK_SIZE/4);
    byte* c = JARENA_alloc(arena, BLOCK_SIZE/4);
    byte* d = JARENA_alloc(arena, BLOCK_SIZE/4);

    assert(a);
    assert(b);
    assert(c);
    assert(d);

    // Later allocations should be at higher addresses
    assert(b > a);
    assert(c > b);
    assert(d > c);

    //Should all be in single block
    assert(!arena->head->next);

    JARENA_free(arena);
}

static void test_allocation_exceeding_block_size(void) {
    JARENA* arena = JARENA_new();

    byte* mem = JARENA_alloc(arena, BLOCK_SIZE + 128);
    assert(mem != NULL);

    // If requested size exceeds default block, should create block of exact requested size
    assert(arena->allocated_bytes == BLOCK_SIZE + 128);
    assert(!arena->head->next);


    /* A large allocation should still be writable */
    memset(mem, 0xAA, BLOCK_SIZE + 128);
    for (size_t i = 0; i < BLOCK_SIZE + 128; i++) {
        assert(mem[i] == 0xAA);
    }

    JARENA_free(arena);
    printf("test_allocation_exceeding_block_size passed\n");
}

static void test_multiple_blocks_created(void) {
    JARENA* arena = JARENA_new();
    assert(arena != NULL);

    /* Force creation of multiple blocks */
    for (int i = 0; i < 9; i++) {
        byte* mem = JARENA_alloc(arena, BLOCK_SIZE/4);
        assert(mem != NULL);
    }

    /* Walk the linked list of blocks */
    int block_count = 0;
    JARENA_NODE* node = arena->head;
    while (node) {
        block_count++;
        node = node->next;
    }

    assert(block_count == 3);


    JARENA_free(arena);
    printf("test_multiple_blocks_created passed\n");
}

static void test_free_arena(void) {
    JARENA* arena = JARENA_new();

    JARENA_alloc(arena, 256);
    JARENA_alloc(arena, 512);

    int result = JARENA_free(arena);
    assert(result == 0);

    printf("test_free_arena passed\n");
}

static void test_general(void) {
    JARENA* arena = JARENA_new();

    char* alloc = (char*) JARENA_alloc(arena, 6);

    strcpy(alloc, "hello");
}

int main(void) {
    test_arena_new();
    test_single_allocation();
    test_allocations_same_block();
    test_allocation_exceeding_block_size();
    test_multiple_blocks_created();
    test_free_arena();

    test_general();
    printf("All JARENA tests passed!\n");
    return 0;
}
