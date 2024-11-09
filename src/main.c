#include <time.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// block counter.
int current_id = 1;

// Block definition
typedef struct Block {
    // Headers
    int content_len;
    int Idl;
    time_t time_stamp;
    unsigned char *previous_hash;
    unsigned char *current_hash;
    // data
    char *data;
    // Linked List
    struct Block *next_block;
} Block;

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
    block->Idl = current_id++;
    // Allocate new address so that when we free we do not free it for both.
    block->previous_hash = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);
    if (previous_block) {
        memcpy(block->previous_hash, previous_block->current_hash, SHA256_DIGEST_LENGTH);
    } else {
        // If it the first Block then the hash will be all zeros
        memset(block->previous_hash, 0, SHA256_DIGEST_LENGTH);
    }
    block->current_hash = create_hash(block);
    block->next_block = NULL;  // No next block initially

    // If there is a previous block, link it to the new block
    // For the linked List.
    if (previous_block != NULL) {
        previous_block->next_block = block;
    }

    return block;
}
void print_block(Block *block) {
    printf("Block ID: %d\n", block->Idl);
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

// Main function demonstrating dynamic block creation and linking
int main() {
    Block *first_block = NULL;
    Block *last_block = NULL;
    char data[256];

    // Create the first block (it will not have a previous block)
    first_block = create_block("Hello World", NULL);
    last_block = first_block;  // Set last_block to point to the first block

    // Ask user to add more blocks dynamically
    while (1) {
        printf("Enter block data (or type 'exit' to stop): ");
        fgets(data, sizeof(data), stdin);
        // Remove the newline character at the end of the input
        data[strcspn(data, "\n")] = '\0';

        if (strcmp(data, "exit") == 0) {
            break;
        }

        // Create a new block, passing the last block to link it
        last_block = create_block(data, last_block);
    }

    // Print the entire blockchain
    Block *current_block = first_block;
    while (current_block != NULL) {
        print_block(current_block);
        current_block = current_block->next_block;
    }

    // Free memory for each block (and its data)
    current_block = first_block;  // Reuse the same pointer for freeing memory
    while (current_block != NULL) {
        Block *temp = current_block;
        current_block = current_block->next_block;

        free(temp->data);
        free(temp->previous_hash);
        free(temp->current_hash);
        free(temp);
    }

    return 0;
}
