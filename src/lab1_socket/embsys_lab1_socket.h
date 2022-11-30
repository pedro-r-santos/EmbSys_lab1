
#ifndef EMBSYS_LAB1_SOCKET_H_
#define EMBSYS_LAB1_SOCKET_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../lab1_error_handler/embsys_lab1_error_handler.h"

extern void* get_client_addr(struct sockaddr* ptr_client_addr);
extern int set_server_info(struct addrinfo* ptr_server_info);

#endif
