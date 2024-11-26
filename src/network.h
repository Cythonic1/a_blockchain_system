
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
#define DEFAULT_PORT_NUMBER_FOR_SOCKET 9988 // for listen
#define BROAD_CAST_PORT_NUMBER 8899

enum Message{
    BLOCK_MESSAGE
};

// struct to define the network protocol.
typedef struct Network_Block{
    int version;
    int content_len ;
    enum Message message_type;
    Block *data;
}Network_Block ;

// THis struct represent the connected node.
typedef struct Connected_Node_Info{
    int fd;
    struct sockaddr_in conn_node;
}Connected_Node_Info;


typedef struct P2P_network{
    Connected_Node_Info *nodes[10];
    uint16_t connecd_nodes_number;

}P2P_network;

// This node represent the current node running on this device.
typedef struct Node{
    int socket;
    uint16_t common_port ;
    struct sockaddr_in address_info;
    socklen_t socket_length;
    // ! i decided to move these two fields into its own struct \
    // because in P2P network we need to have all of our connected nodes in one place \
    // havin them inside this struct is not a good idea since it just gonna increase the overhead \
    // and also the mutual lock for multi threading since \
    // we need to handel both from lisnter and connector \

    // Connected_Node_Info *nodes[10];
    // uint16_t connecd_nodes_number;
}Node;

// a buffer struct to make it easier to work with bytes.
typedef struct Buffer{
    int size;
    unsigned char *buffer;
}Buffer;

// int create_socket(int port, char *ip);
Buffer *serilized_network(Network_Block *packet);
Network_Block *deserilized_network(Buffer *buffer);
void print_packet(Network_Block *packet);
void *discover_nodes();
#endif
