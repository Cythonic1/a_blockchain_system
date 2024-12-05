#include "./block.h"
#include <openssl/evp.h>





// block counter.
int current_id = 1;

// Block definition

// unsigned char *create_hash(Block* block) {
//     // Allocate memory for the hash (SHA256 length)
//     unsigned char *hash_buffer = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);
//     if (!hash_buffer) {
//         perror("Failed to allocate memory for hash");
//         return NULL;
//     }

//     SHA256_CTX sha256;
//     SHA256_Init(&sha256);
//     SHA256_Update(&sha256, (unsigned char *)block, sizeof(*block));
//     SHA256_Final(hash_buffer, &sha256);

//     // Print the hash
//     printf("SHA-256 hash of struct: ");
//     for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
//         printf("%02x", hash_buffer[i]);
//     }
//     printf("\n");

//     return hash_buffer;
// }
unsigned char *create_hash(Block *block) {
    // Allocate memory for the hash (SHA256 length)
    unsigned char *hash_buffer = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);
    if (!hash_buffer) {
        perror("Failed to allocate memory for hash");
        return NULL;
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();  // Create a new context
    if (mdctx == NULL) {
        perror("EVP_MD_CTX_new failed");
        free(hash_buffer);
        return NULL;
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {  // Initialize the SHA256 context
        perror("EVP_DigestInit_ex failed");
        EVP_MD_CTX_free(mdctx);
        free(hash_buffer);
        return NULL;
    }

    // Update the hash with the data (similar to SHA256_Update)
    if (EVP_DigestUpdate(mdctx, (unsigned char *)block, sizeof(*block)) != 1) {
        perror("EVP_DigestUpdate failed");
        EVP_MD_CTX_free(mdctx);
        free(hash_buffer);
        return NULL;
    }

    // Finalize the hash and store the result in hash_buffer
    unsigned int len = SHA256_DIGEST_LENGTH;  // Length of the hash
    if (EVP_DigestFinal_ex(mdctx, hash_buffer, &len) != 1) {
        perror("EVP_DigestFinal_ex failed");
        EVP_MD_CTX_free(mdctx);
        free(hash_buffer);
        return NULL;
    }

    // Clean up the context
    EVP_MD_CTX_free(mdctx);

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
    block->content_len = strlen(data) + 1; // Derfrence to get the actual size.
    
    block->data = (char *) malloc(block->content_len);
    strncpy(block->data, data, block->content_len);
    block->time_stamp = time(NULL);
    block->Id = current_id++;
    if (previous_block) {
        memcpy(block->previous_hash, previous_block->current_hash, SHA256_DIGEST_LENGTH);
    } else {
        // If it the first Block then the hash will be all zeros
        memset(block->previous_hash, 0, SHA256_DIGEST_LENGTH);
    }
    unsigned char *hash_created = create_hash(block);
    memcpy(block->current_hash,hash_created , SHA256_DIGEST_LENGTH);
    block->next_block = NULL;  // No next block initially

    // If there is a previous block, link it to the new block
    // For the linked List.
    if (previous_block != NULL) {
        previous_block->next_block = block;
    }

    free(hash_created);
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
