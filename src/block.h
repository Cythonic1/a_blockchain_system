
#ifndef BLOCK_H
#define BLOCK_H

#include <time.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Block {
    // Headers
    int Id;
    time_t time_stamp;
    int content_len;
    unsigned char current_hash[SHA256_DIGEST_LENGTH];
    unsigned char previous_hash[SHA256_DIGEST_LENGTH];
    // data
    char *data;
    // Linked List
    struct Block *next_block;
} Block;

// Function declarations
unsigned char *create_hash(Block *block);
Block *create_block(char *data, Block *previous_block);
void print_block(Block *block);

#endif
