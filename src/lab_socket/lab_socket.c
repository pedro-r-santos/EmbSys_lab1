#include "lab_socket.h"

/* Get Client address. */
extern inline void* get_client_addr(struct sockaddr* ptr_client_addr) {
  return (ptr_client_addr->sa_family == AF_INET)
             ? (void*)&((struct sockaddr_in*)ptr_client_addr)->sin_addr    /* IPv4 addr. */
             : (void*)&((struct sockaddr_in6*)ptr_client_addr)->sin6_addr; /* Ipv6 addr. */
}

extern int set_server_info(struct addrinfo* ptr_server_info) {
  void* ret_val = memset(ptr_server_info, 0, sizeof(struct addrinfo));
  if (ret_val == NULL) {
    return EXIT_FAILURE;
  }
  ptr_server_info->ai_family = AF_INET6;      /* Use IPv6. */
  ptr_server_info->ai_socktype = SOCK_STREAM; /* Use TCP. */
  ptr_server_info->ai_flags = AI_PASSIVE;     /* The OS defines our IP address. */
  return EXIT_SUCCESS;
}
