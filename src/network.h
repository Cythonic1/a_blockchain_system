
#ifndef NETWORK_H
#define  NETWORK_H
#include "./block.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum Message{
    BLOCK_MESSAGE
};

typedef struct Network_Block{
    int version;
    int content_len ;
    enum Message message_type;
    Block *data;
}Network_Block ;

// int create_socket(int port, char *ip);
unsigned char *serilized_network(Network_Block *packet);
Network_Block *deserilized_network(unsigned char *buffer);
void print_packet(Network_Block *packet);
#endif
