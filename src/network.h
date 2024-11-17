
#ifndef NETWORK_H
#define  NETWORK_H
#include "./block.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#define CREATE_SOCKET_ERROR -101
#define BIND_SOCKET_ERROR -102
#define NULL_NODE -103
#define LISTEN_SOCKET_ERROR -104
#define MEMORY_ALLOCATION_ERROR -201
#define DEFAULT_PORT_NUMBER_FOR_SOCKET 9988

enum Message{
    BLOCK_MESSAGE
};

typedef struct Network_Block{
    int version;
    int content_len ;
    enum Message message_type;
    Block *data;
}Network_Block ;

// Not sure if the best idea !!.
typedef struct Connected_Node_Info{
    int fd;
    struct sockaddr_in conn_node;
}Connected_Node_Info;
typedef struct Node{
    int socket;
    uint16_t common_port ;
    struct sockaddr_in address_info;
    socklen_t socket_length;
    Connected_Node_Info *nodes[10];
    uint16_t connecd_nodes_number;
}Node;
// int create_socket(int port, char *ip);
unsigned char *serilized_network(Network_Block *packet);
Network_Block *deserilized_network(unsigned char *buffer);
void print_packet(Network_Block *packet);
#endif
