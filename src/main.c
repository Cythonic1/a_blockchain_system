#include "./block.h"
#include "./network.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

volatile int running = 1;
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
    P2P_network *network = malloc(sizeof(P2P_network));
    if (network == NULL) {
        perror("Failed to allocate memory for P2P_network");
        return EXIT_FAILURE;
    }

    pthread_t  thread;
    if (pthread_create(&thread,NULL, discover_nodes, NULL ) != 0 ){
        perror("Error while creating thread to discover nodes\n");
    }
    pthread_t  thread1;
    if (pthread_create(&thread1,NULL, listen_for_new_nodes, network ) != 0 ){
        perror("Error while creating thread to discover nodes\n");
    }
    // Ask user to add more blocks dynamically
    while (1) {
        packet->data = last_block;
        packet->content_len = last_block->content_len + (2 * sizeof(int)) + sizeof(time_t) + (2 * SHA256_DIGEST_LENGTH);  // Update content_len
        printf("-------------------------------Network serialized ------------------\n");
        Buffer *network_buffer = serilized_network(packet);
        // Print the serialized data
        for (int i = 0; i < packet->content_len; i++) {
            printf("%02x", network_buffer->buffer[i]);
        }
        printf("\n");

        // Deserialize and print the packet
        Network_Block *tmp = deserilized_network(network_buffer);
        print_packet(tmp);

        // Free the deserialized network block
        free(tmp->data->data);
        free(tmp->data);
        free(tmp);

        // Free the serialized network buffer
        free(network_buffer->buffer); 
        free(network_buffer);

        printf("Enter block data (or type 'exit' to stop): ");
        fgets(data, sizeof(data), stdin);
        // Remove the newline character at the end of the input
        data[strcspn(data, "\n")] = '\0';

        if (strcmp(data, "exit") == 0) {
            pthread_cancel(thread);
            pthread_cancel(thread1);
            break;
        }

        // Create a new block, passing the last block to link it
        last_block = create_block(data, last_block);
    }
    // Print the entire blockchain
    Block *current_block = first_block;
    // while (current_block != NULL) {
    //     print_block(current_block);
    //     current_block = current_block->next_block;
    // }

    // Free memory for each block (and its data)
    current_block = first_block;  // Reuse the same pointer for freeing memory
    while (current_block != NULL) {
        Block *temp = current_block;
        current_block = current_block->next_block;
        free(temp->data);
        free(temp);
    }
    free(network);
    free(packet);

    // Join threads to ensure proper cleanup
    pthread_join(thread, NULL);
    pthread_join(thread1, NULL);
    return 0;
}
