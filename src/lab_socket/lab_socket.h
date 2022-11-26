
#ifndef LAB_SOCKET_H_
#define LAB_SOCKET_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

extern inline void* get_client_addr(struct sockaddr* ptr_client_addr);
extern int set_server_info(struct addrinfo* ptr_server_info);

#endif
