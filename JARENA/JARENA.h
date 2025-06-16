# ifndef JARENA_H
# define JARENA_H
# include<stddef.h>
/* default size of each allocated arena block */
#define BLOCK_SIZE 4096

typedef unsigned char byte;

typedef struct JARENA_NODE {
    /* A pointer to the block of memory*/
    byte* mem_block;

    /* A pointer to the next address of free memory in the block. */
    byte* cur_block_address;

    /* The size of this block. Could differ from BLOCK_SIZE if this node holds a data structure*/
    size_t size;

    /* Pointer to next node with allocated memory block. Used when freeing the arena. */
    struct JARENA_NODE* next;

} JARENA_NODE;

typedef struct JARENA {
    /* Pointer to first node with allocated arena block */
    JARENA_NODE* head;

    /* Pointer to the newest allocated arena block*/
    JARENA_NODE* tail;

    /* Number of bytes allocated in this arena */
    size_t allocated_bytes;
} JARENA;


JARENA* JARENA_new(void);

byte* JARENA_alloc(JARENA* arena, size_t alloc_amount);


int JARENA_free(JARENA* arena);
# endif