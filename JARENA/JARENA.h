# ifndef JARENA_H
# define JARENA_H

/// @file JARENA.h
# include<stddef.h>
#include<stdlib.h>
#include<string.h>

#define BLOCK_SIZE 4096 ///< default size of each allocated arena block

typedef unsigned char byte; ///< a byte in memory

/**
 * A node in the arena data structure. Holds a block of allocated memory
 */
typedef struct JARENA_NODE {
    /// Pointer to the block of memory this node holds
    byte* mem_block;

    /// Pointer to the next byte of open memory held by this node
    byte* cur_block_address;

    /// The size memory held by this block. Could differ from BLOCK_SIZE if specified
    size_t size;

    /// Pointer to next node with allocated memory block. Used when freeing the arena.
    struct JARENA_NODE* next;

} JARENA_NODE;


/**
 * The arena data structure. Holds a linked list of allocated memory blocks
 */
typedef struct JARENA {
    /// First node of allocated memory
    JARENA_NODE* head;

    /// Pointer to the newest allocated block
    JARENA_NODE* tail;

    /// Total number of bytes allocated in this arena
    size_t allocated_bytes;
} JARENA;

/**
 * Allocates a new arena data structure
 * \return A pointer to the new arena
 */
JARENA* JARENA_new(void);

/**
 * Allocates a new block of memory in the arena
 * @param[in, out] arena
 * @param[in] alloc_amount
 * \return A pointer to the newly allocated block of memory
 */

byte* JARENA_alloc(JARENA* arena, size_t alloc_amount);

/**
 * Frees all memory allocated in the arena
 * @param[in, out] arena
 * \return 0 on success, anything else on failure
 */
int JARENA_free(JARENA* arena);
# endif