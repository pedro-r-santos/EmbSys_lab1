
#ifndef EMBSYS_LAB1_SOCKET_H_
#define EMBSYS_LAB1_SOCKET_H_

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static const int CONTINUE = -100;
static const unsigned int MAX_CLIENT_DATA = 100;
extern void* get_client_addr(struct sockaddr* ptr_client_addr);
extern int set_server_protocol(struct addrinfo* ptr_server_protocols);
extern int set_server_addr_info(struct addrinfo* ptr_server_protocols, struct addrinfo** ptr_server_info);
extern int get_server_file_descriptor(int* server_file_descriptor, struct addrinfo* server_info);
extern int close_communication(const int* file_descriptor);
extern int set_listen(const int* server_file_descriptor);
extern int set_client_file_descriptor(const int* server_file_descriptor, int* client_file_descriptor,
                                      char* client_ip_addr);
extern int get_client_data(const int* client_file_descriptor, char* client_data);

#endif
