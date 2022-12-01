
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

#include "../lab1_error_message/embsys_lab1_error_message.h"

static const int MEMSET_ERROR = -1;
static const int SET_SERVER_FILE_DESCRIPTOR_ERROR = -2;
extern void* get_client_addr(struct sockaddr* ptr_client_addr);
extern int set_server_protocol(struct addrinfo* ptr_server_protocols);
extern int set_server_addr_info(struct addrinfo* ptr_server_protocols, struct addrinfo** ptr_server_info);
extern int get_server_file_descriptor(int* server_file_descriptor, struct addrinfo* server_info);
extern int close_communication(const int* file_descriptor);
extern int set_listen(const int* server_file_descriptor);

#endif
