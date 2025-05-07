# JLIB ARENA

A data structure used to simplify memory management

## `JARENA_new(void)`

Returns pointer to a new arena. *Always call JARENA_free() when you are done with it*

## `JARENA_alloc(JARENA* arena, size_t alloc_len, size_t size_element)`

Returns a chunk of bytes of the specified size. Element size and allocation length are separated to check for overlow, similar to calloc.

## `JARENA_realloc`

- Shouldn't be used for complex data structures that need resizing

The realloc functionality is very rudimentary, it simply grabs a new chunk of memory and copies the original data there. The original location isn't freed.

## `JARENA_free`

Frees all memory allocated through the JARENA_alloc function. The main incentive for allocating memory this way, you don't need to keep track of what you have allocated as the data structure handles it.
