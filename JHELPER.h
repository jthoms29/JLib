#include <stdlib.h>
#include <stdbool.h>

#define CAST_INT(x) ((void*)(intptr_t)(x))
#define RETRV_INT(p) ((int)(intptr_t)(p))
/* Get current time in milliseconds */
long cur_ms();

/* Generate a random string of supplied length */
void rand_string(char* str, size_t len);


/* Generate num unique strings of len strlens */
char** gen_unique_strings(size_t num, size_t strlens);


/* Free strings allocated with gen_unique_strings */
void free_unique_strings(char** strs, size_t num);


/* Hashing function for strings */
size_t string_hash(void* key);


/* Hash function to test collision behavior */
size_t force_collide(void* key);


/* Comparator function for strings */
bool string_compare(void* key1, void* key2);


size_t int_hash(void* key);

bool int_compare(void* key1, void* key2);