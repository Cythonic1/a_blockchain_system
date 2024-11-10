#include "./block.h"
#include "serilaization.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char *serilized(Block *block) {
    int buffer_size = (2 * SHA256_DIGEST_LENGTH) + block->content_len + sizeof(time_t) + (2 * sizeof(int));

    unsigned char *buffer = (unsigned char *)malloc(buffer_size);
    unsigned char *ptr = buffer;

    // Set the fixed values.
    memcpy(ptr, &block->Id, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &block->time_stamp, sizeof(time_t));
    ptr += sizeof(time_t);
    memcpy(ptr, &block->content_len, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &block->current_hash, SHA256_DIGEST_LENGTH);
    ptr += SHA256_DIGEST_LENGTH;
    memcpy(ptr, &block->previous_hash, SHA256_DIGEST_LENGTH);
    ptr += SHA256_DIGEST_LENGTH;
    memcpy(ptr, block->data, block->content_len);

    return buffer;
}

Block *deserilized(unsigned char *buffer) {
    Block *block = (Block *)malloc(sizeof(Block));

    unsigned char *ptr = buffer;

    memcpy(&block->Id, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&block->time_stamp, ptr, sizeof(time_t));
    ptr += sizeof(time_t);
    memcpy(&block->content_len, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&block->current_hash, ptr, SHA256_DIGEST_LENGTH);
    ptr += SHA256_DIGEST_LENGTH;
    memcpy(&block->previous_hash, ptr, SHA256_DIGEST_LENGTH);
    ptr += SHA256_DIGEST_LENGTH;

    // Allocate memory for data and copy
    block->data = (char *)malloc(block->content_len + 1);
    memcpy(block->data, ptr, block->content_len);
    block->data[block->content_len] = '\0';

    // print_block(block);
    return block;
}
