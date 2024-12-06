#include "./network.h"
#include <pthread.h>
#include <unistd.h>
#include "./serilaization.h"
#include "block.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
// typedef struct Network_Block{
//     int version;
//     int content_len ;
//     enum Message message_type;
//     Block *data;
// }Network_Block ;



P2P_network *init_p2p(){
    P2P_network *p2p  = (P2P_network *) malloc(sizeof(P2P_network));
    p2p->connecd_nodes_number = 0;
    return p2p;
}

// i should make sure that the packet contains data before serilize it.
Buffer *serilized_network(Network_Block *packet){
    // Here we allocate dynamic twice one for the struct it self and \
    // one for the char pointer within the struct
    Buffer *buf = (Buffer *)malloc(sizeof(Buffer));
    int buffer_len = (sizeof(int) * 2) + sizeof(packet->message_type) + packet->content_len;

    buf->buffer = (unsigned char *)malloc(buffer_len);
    buf->size = buffer_len;
    
    unsigned char *ptr = buf->buffer;
    memcpy(ptr, &packet->version, sizeof(int));
    ptr+= sizeof(int);
    memcpy(ptr, &packet->content_len, sizeof(int));
    ptr+= sizeof(int);
    memcpy(ptr, &packet->message_type, sizeof(packet->message_type));
    ptr+= sizeof(packet->message_type);
    unsigned char *packet_data = serilized(packet->data);
    memcpy(ptr,  packet_data , packet->content_len);
    free(packet_data);
    return buf;
}

Network_Block *deserilized_network(Buffer *buffer){
    Network_Block *packet = (Network_Block *)malloc(sizeof(Network_Block));
        unsigned char *ptr = buffer->buffer;

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

        return packet;
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


// SO this function will set up a listener on a common port for nodes that want to connect.
// TODO IMplement clean up handler
void make_socket_listen(Node *node, int backlog, P2P_network *p2p){
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
            Connected_Node_Info *new_node = (Connected_Node_Info *)malloc(sizeof(Connected_Node_Info)); 
            // connect will return file discriptor of the connected node
            new_node->fd = client;
            // and this will be the info of the next node;
            new_node->conn_node = next_node_tmp;
            // The array will take an address refres to the new connected nodes.
            p2p->nodes[p2p->connecd_nodes_number++] = new_node;
            // Next a new thread should be created an move this data to that thread;
        };
    }
}



// Since we are serilizing the Network block message we just
// need to get the buffer thats hold the value
// Also its a good idea to free the unsigned char buffer;
void broadcast(Node *node, Buffer *serilized_message, P2P_network *p2p){
    if(node == NULL){
        perror("node is null can not broadcast the block");
        exit(NULL_NODE);
    }

    for(int i = 0; i < p2p->connecd_nodes_number ; i++){
        // TODO: the size of the buffer i have not calculatd yet it should be input too for the funtion
        // For now we just gonna stick with this.
        int number_of_sent_bytes = send(p2p->nodes[i]->fd, serilized_message->buffer, serilized_message->size, 0);
    }
    printf("The block has been broadcast");
    free(serilized_message);
}



// I actually make it easy for my self since my p2p->nodes array holds the other node
// inforamtion i can just refres to each other node IP in order to connect to them.
// NOTE implement this you should keep in mind the to add mute lock and figure out later 
// do we need to open it as new threat or run it on the same threat as the listen_for_new_nodes function.
void connecter(){
    // TODO: First am thinking of making a function that listen on some port \
    // and this should be common between all nodes so they can find themself on that port.
}

// Make this function so that it send a broadcast message on UDP 
// When other node receives this message they replay with their IP \
// So we can establish TCP connection. and also we need to validate the node\
// so that we can be sure that this node is not malicious
//  Why UDP, As you may know TCP does not support broadcast by nature since they are \
//  connectionful network protocol. you can referes to broadcasr function to see how we can \
//  send broadcast via TCP.
// Refrence : https://cs.baylor.edu/~donahoo/practical/CSockets/code/BroadcastSender.c
// https://cs.baylor.edu/~donahoo/practical/CSockets/code/BroadcastReceiver.c
// TODO: Fix the returns values .
// TODO: Implement the udp message which include the node inforamtion. 
void *discover_nodes(){
    printf("Am starting as new thread \n");
    int discover_socket ;
    struct sockaddr_in broadcast;
    int broadcasrPermission = 1;
    char *string = "ping";
    int buffer_len = strlen(string);


    if ((discover_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Erorr while creating broadcasr socket ");
        return NULL;
    }

    // Setting the udp socket to allow broadcast
    if((setsockopt(discover_socket, SOL_SOCKET, SO_BROADCAST,(void *) &broadcasrPermission, sizeof(int))) < 0){
        perror("Error while setting the socket");
        return NULL;
    }
    broadcast.sin_port = htons(BROAD_CAST_PORT_NUMBER);
    broadcast.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    broadcast.sin_family = AF_INET;
    memset(&broadcast.sin_zero, 0, sizeof(broadcast.sin_zero));

    while(1){
        if(sendto(discover_socket, string, buffer_len, 0, (struct sockaddr *) &broadcast, sizeof(struct sockaddr)) != buffer_len){
            perror("Some Error occurs");
            return NULL;
        }
        sleep(3);

    }


}


int get_current_host_IP(const char *interface, char *ip_address, size_t size) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("Cannot create the socket to get the IP address");
        return -1;
    }

    struct ifreq ifr;  // Allocate on the stack, no malloc needed

    // Copy the interface name into the ifreq structure
    strncpy(ifr.ifr_name, interface, IF_NAMESIZE - 1);
    ifr.ifr_name[IF_NAMESIZE - 1] = '\0'; 

    // Get the IP address of the interface
    if (ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
        perror("Error in getting IP via IOCTL");
        close(fd);
        return -1;
    }

    close(fd);

    // Extract the IP address into the provided buffer
    struct sockaddr_in *ip = (struct sockaddr_in *)&ifr.ifr_addr;
    if (!inet_ntop(AF_INET, &ip->sin_addr, ip_address, size)) {
        perror("inet_ntop failed");
        return -1;
    }

    return 0;
}


void clean_up_listen_for_new_nodes(void *args){
    Buffer *buf = (Buffer *) args;

    if(buf != NULL){
        if(buf->buffer != NULL) {
            free(buf->buffer);
        }
        free(buf);
    }
    printf("Cleanup completed.\n");
}
//TODO implement adding the found device into our p2p network
void *listen_for_new_nodes(void *arg){
    printf("am listen_for_new_nodes : \n");
    // P2P_network *network = (P2P_network *) arg;
    int rec_socket ;
    struct sockaddr_in discovered_node;
    struct sockaddr_in new_client;
    socklen_t clien_address_len = sizeof(struct sockaddr_in);
    Buffer *buffer = (Buffer *) malloc(sizeof(Buffer));
    char current_ip[INET_ADDRSTRLEN];
    int rec_string_len;

    if(buffer == NULL){
        perror("Error while allocating for listen function");
        exit(MEMORY_ALLOCATION_ERROR);
    }
    buffer->buffer = (unsigned char *) malloc(255);
    if(buffer->buffer == NULL){
        perror("Error while allocating for listen function");
        free(buffer);
        exit(MEMORY_ALLOCATION_ERROR);
    }
    
    pthread_cleanup_push(clean_up_listen_for_new_nodes, buffer);
    if((rec_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
        perror("Error while creating listen socket");
        free(buffer->buffer);
        free(buffer);
        exit(CREATE_SOCKET_ERROR);
    }
    discovered_node.sin_family = AF_INET;
    discovered_node.sin_port = htons(BROAD_CAST_PORT_NUMBER);
    discovered_node.sin_addr.s_addr = htonl(INADDR_ANY);
   memset(&discovered_node.sin_zero, 0, sizeof(discovered_node.sin_zero));
   
    // Get current host IP
    if (get_current_host_IP("eth0", current_ip, sizeof(current_ip)) != 0) {
        perror("Failed to retrieve current host IP");
        free(buffer->buffer);
        free(buffer);
        close(rec_socket);
        exit(EXIT_FAILURE);
    }

    if(bind(rec_socket, (struct sockaddr *) &discovered_node, sizeof(struct sockaddr)) < 0){
        free(buffer->buffer);
        free(buffer);
        perror("Something went wrong while bind the listener of the udp");
    }

    while (1) {
        if((buffer->size = recvfrom(rec_socket, buffer->buffer, 255, 0, (struct sockaddr *)&new_client, &clien_address_len)) < 0){
            perror("Error while reciving from the client via udp");
            // NOTE HERE.
            free(buffer->buffer);
            free(buffer);
        }

        char client_ip[INET_ADDRSTRLEN];
        if (!inet_ntop(AF_INET, &new_client.sin_addr, client_ip, sizeof(client_ip))) {
            perror("inet_ntop failed");
            continue;
        }

        if (strcmp(current_ip, client_ip) == 0) {
            // printf("Skipping the node, it's the current host\n");
            continue;
        }


        printf("new client connected from %s:%i", inet_ntoa(new_client.sin_addr), ntohs(new_client.sin_port));
        buffer->buffer[buffer->size] = '\0';
        printf("\n recive from client : %s \n", buffer->buffer);
    }
    close(rec_socket);
    pthread_cleanup_pop(1);
    return NULL;
}
