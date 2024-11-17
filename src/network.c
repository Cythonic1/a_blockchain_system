#include "./network.h"
#include "./serilaization.h"
#include "block.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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

// Create a function to create a socket
Node *socket_gen(int lport){

    Node *node = malloc(sizeof(Node));
    node->connecd_nodes_number = 0;
    if(node == NULL){
        perror("Error while allocating memory for the node");
        exit(MEMORY_ALLOCATION_ERROR);
    }
    node->socket_length = sizeof(node->address_info);
    if(lport > 65535 || lport < 0 ){
        node->common_port = DEFAULT_PORT_NUMBER_FOR_SOCKET;
    }else{
        node->common_port = lport;
    }

    if ((node->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        node->socket = CREATE_SOCKET_ERROR;
        perror("Error while createing socket");
        exit(CREATE_SOCKET_ERROR);
    }
    node->address_info.sin_addr.s_addr = INADDR_ANY;
    node->address_info.sin_port = htons(node->common_port);
    node->address_info.sin_family = AF_INET;
    memset(&node->address_info.sin_zero, 0, sizeof(node->address_info.sin_zero));

    if(bind(node->socket, (struct sockaddr *) &node->address_info, node->socket_length) < 0){
        perror("Error while binding");
        exit(BIND_SOCKET_ERROR);
    }
    return node;
}

void make_socket_listen(Node *node, int backlog){
    // This represnt the new connected node
    struct sockaddr_in next_node_tmp;
    int client;
    socklen_t socket_length = sizeof(next_node_tmp);
    if(node == NULL){
        perror("node is null can not listen");
        exit(NULL_NODE);
    }

    if(listen(node->socket, backlog) < 0){
        perror("Error while listen");
        exit(LISTEN_SOCKET_ERROR);
    }
    printf("the Socket listen on %s:%i\n",inet_ntoa(node->address_info.sin_addr) , ntohs(node->common_port));
    // This should be in a single thread where this thread will update the node->nodes array and
    // hopfully we are able to access it later so that we can broadcast a message.
    while (1){
        // This will return the file discriptor of the new connected node.
        if((client = accept(node->socket, (struct sockaddr *) &next_node_tmp, &socket_length)) < 0){
            Connected_Node_Info *new_node = malloc(sizeof(Connected_Node_Info));
            new_node->fd = client;
            new_node->conn_node = next_node_tmp;
            // The array will take an address refres to the new connected nodes.
            node->nodes[node->connecd_nodes_number++] = new_node;
            // Next a new thread should be created an move this data to that thread;
        };
    }
}



// Since we are serilizing the Network block message we just
// need to get the buffer thats hold the value
// Also its a good idea to free the unsigned char buffer;
void broadcast(Node *node, unsigned char *serilized_message){
    if(node == NULL){
        perror("node is null can not broadcast the block");
        exit(NULL_NODE);
    }

    for(int i = 0; i < node->connecd_nodes_number ; i++){
        // TODO: the size of the buffer i have not calculatd yet it should be input too for the funtion
        // For now we just gonna stick with this.
        int number_of_sent_bytes = send(node->nodes[i]->fd, serilized_message, 1024, 0);
    }
    printf("The block has been broadcast");
    free(serilized_message);
}



// I actually make it easy for my self since my node->nodes array holds the other node
// inforamtion i can just refres to each other node IP in order to connect to them.
