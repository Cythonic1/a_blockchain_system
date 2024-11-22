#include "./block.h"
#include "./network.h"
#include <stdio.h>
#include <stdlib.h>
// Main function demonstrating dynamic block creation and linking
int main() {
    Block *first_block = NULL;
    Block *last_block = NULL;
    char data[256];
    Network_Block *packet = (Network_Block *)malloc(sizeof(Network_Block));
    packet->message_type = BLOCK_MESSAGE;
    packet->version = 1;

    // Create the first block (it will not have a previous block)
    first_block = create_block("Hello World", NULL);
    last_block = first_block;  // Set last_block to point to the first block

    // Ask user to add more blocks dynamically
    while (1) {
        packet->data = last_block;
        packet->content_len = last_block->content_len + (2*sizeof(int)) + sizeof(time_t) + (2 * SHA256_DIGEST_LENGTH);  // Update content_len
        printf("-------------------------------Network serilized ------------------\n");
        Buffer *network_buffer = serilized_network(packet);
        for (int i = 0; i < packet->content_len; i++) {
            printf("%02x", network_buffer->buffer[i]);
        }
        printf("\n");
        Network_Block *tmp = deserilized_network(network_buffer);
        print_packet(tmp);

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
        free(temp);
    }

    free(packet);
    return 0;
}
