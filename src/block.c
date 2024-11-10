#include "./block.h"




// block counter.
int current_id = 1;

// Block definition

unsigned char *create_hash(Block* block) {
    // Allocate memory for the hash (SHA256 length)
    unsigned char *hash_buffer = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);
    if (!hash_buffer) {
        perror("Failed to allocate memory for hash");
        return NULL;
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, (unsigned char *)block, sizeof(*block));
    SHA256_Final(hash_buffer, &sha256);

    // Print the hash
    printf("SHA-256 hash of struct: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash_buffer[i]);
    }
    printf("\n");

    return hash_buffer;
}

Block *create_block(char *data, Block *previous_block){

    Block *block = (Block *)malloc(sizeof(Block));
    block->content_len = strlen(data); // Derfrence to get the actual size.
    block->data = (char *) malloc(block->content_len +1 );
    strncat(block->data, data, block->content_len);
    block->time_stamp = time(NULL);
    block->Id = current_id++;
    if (previous_block) {
        memcpy(block->previous_hash, previous_block->current_hash, SHA256_DIGEST_LENGTH);
    } else {
        // If it the first Block then the hash will be all zeros
        memset(block->previous_hash, 0, SHA256_DIGEST_LENGTH);
    }
    memcpy(block->current_hash,create_hash(block) , SHA256_DIGEST_LENGTH);
    block->next_block = NULL;  // No next block initially

    // If there is a previous block, link it to the new block
    // For the linked List.
    if (previous_block != NULL) {
        previous_block->next_block = block;
    }

    return block;
}
void print_block(Block *block) {
    printf("Block ID: %d\n", block->Id);
    printf("Block Content: %s\n", block->data);
    printf("Block Timestamp: %ld\n", block->time_stamp);
    printf("Block Content Length: %d\n", block->content_len);
    printf("Previous Hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", block->previous_hash[i]);
    }
    printf("\n");
    printf("Current Hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", block->current_hash[i]);
    }
    printf("\n");
}
