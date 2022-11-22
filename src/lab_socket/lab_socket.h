
#ifndef LAB_SOCKET_H_
#define LAB_SOCKET_H_ 

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

extern inline void *get_client_addr(struct sockaddr*);
extern struct addrinfo* set_server_definition(void);

#endif
