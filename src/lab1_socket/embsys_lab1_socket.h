
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

extern void* get_client_addr(struct sockaddr* ptr_client_addr);
extern int set_server_protocols(struct addrinfo* ptr_server_info);
extern int* set_server_file_descriptor(struct addrinfo* ptr_server_info);
extern int close_communications(const int* file_descriptor);

#endif
