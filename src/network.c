#include "./network.h"
#include "./serilaization.h"
#include "block.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// typedef struct Network_Block{
//     int version;
//     int content_len ;
//     enum Message message_type;
//     Block *data;
// }Network_Block ;


// i should make sure that the packet contains data before serilize it.
unsigned char *serilized_network(Network_Block *packet){
    // int data_size = packet->data->content_len;
    int buffer_len = (sizeof(int) * 2) + sizeof(packet->message_type) + packet->content_len;
    unsigned char *buffer = (unsigned char *)malloc(buffer_len);
    unsigned char *ptr = buffer;

    memcpy(ptr, &packet->version, sizeof(int));
    ptr+= sizeof(int);
    memcpy(ptr, &packet->content_len, sizeof(int));
    ptr+= sizeof(int);
    memcpy(ptr, &packet->message_type, sizeof(packet->message_type));
    ptr+= sizeof(packet->message_type);
    memcpy(ptr,  serilized(packet->data) , packet->content_len);
    return buffer;
}

Network_Block *deserilized_network(unsigned char *buffer){
    Network_Block *packet = (Network_Block *)malloc(sizeof(Network_Block));
        unsigned char *ptr = buffer;

        memcpy(&packet->version, ptr, sizeof(int));
        ptr += sizeof(int);
        memcpy(&packet->content_len, ptr, sizeof(int));
        ptr += sizeof(int);
        memcpy(&packet->message_type, ptr, sizeof(packet->message_type));
        ptr += sizeof(packet->message_type);

        if (packet->content_len > 0) {
            // Allocate memory for data_buffer with error checking
            unsigned char *data_buffer = (unsigned char *)malloc(packet->content_len);
            if (data_buffer == NULL) {
                free(packet);
                return NULL;
            }

            // Copy data, ensuring it doesn't exceed buffer size
            memcpy(data_buffer, ptr, packet->content_len);

            // Deserialize the Block (data field)
            packet->data = deserilized(data_buffer);
            free(data_buffer);
        } else {
            packet->data = NULL;
        }

        return packet;;
}

void print_packet(Network_Block *packet){
    printf("version : %i\n", packet->version);
    printf("content len : %i\n", packet->content_len);
    printf("message type : %i\n", packet->message_type);
    print_block(packet->data);
    printf("\n");
}
