#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "./JHELPER.h"

/* Get current time in milliseconds */
long cur_ms() {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts)) {
        perror("clock_gettime");
    }
    return ts.tv_sec * 1000 + ts.tv_nsec/1000000;
}

/* Generate a random string of supplied length */
void rand_string(char* str, size_t len) {
    const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789!";

    for (size_t i = 0; i < len; i++) {
        int idx = rand() % (sizeof(charset)-1);
        str[i] = charset[idx];
    }
    str[len] = 0;
}

/* Generate num unique strings of len strlens */
char** gen_unique_strings(size_t num, size_t strlens) {
    char** strs = malloc(num * sizeof(char*));

    for (size_t i = 0; i < num; i++) {
        char rand_part[strlens-2];
        rand_string(rand_part, sizeof(rand_part)-1);

        int len = snprintf(NULL, 0, "%zu_%s", i, rand_part);
        strs[i] = malloc(len+1);
        // make sure unique
        snprintf(strs[i], len + 1, "%zu_%s", i, rand_part);
    }
    return strs;
}

/* Free strings allocated with gen_unique_strings */
void free_unique_strings(char** strs, size_t num) {
    for (size_t i = 0; i < num; i++) {
        free(strs[i]);
    }
    free(strs);
} 

/* Hashing function for strings */
size_t string_hash(void* key) {
    unsigned char* str = key;
    size_t hash = 1469598103934665603ULL;

    while (*str) {
        hash ^= (size_t)*str++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

/* Hash function to test collision behavior */
size_t force_collide(void* key) {
    return 21;
}

/* Comparator function for strings */
bool string_compare(void* key1, void* key2) {
    return strcmp((char*)key1, (char*)key2) == 0;
}

size_t int_hash(void* key) {
    int num = RETRV_INT(key);
    return (size_t) num;
}

bool int_compare(void* key1, void* key2) {
    return RETRV_INT(key1) == RETRV_INT(key2);
}