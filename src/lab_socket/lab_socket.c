#include "lab_socket.h"

/* Get Client address. */
extern inline void* get_client_addr(struct sockaddr* ptr_client_addr) {
  return (ptr_client_addr->sa_family == AF_INET)
             ? (void*)&((struct sockaddr_in*)ptr_client_addr)->sin_addr    /* IPv4 addr. */
             : (void*)&((struct sockaddr_in6*)ptr_client_addr)->sin6_addr; /* Ipv6 addr. */
}

extern struct addrinfo* set_server_definition(void) {
  struct addrinfo* server_definition = NULL;
  memset(server_definition, 0, sizeof(struct addrinfo));
  server_definition->ai_family = AF_INET6;      /* Use IPv6. */
  server_definition->ai_socktype = SOCK_STREAM; /* Use TCP. */
  server_definition->ai_flags = AI_PASSIVE;     /* The OS defines our IP address. */
  return server_definition;
}
