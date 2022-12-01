#include "embsys_lab1_socket.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

static const int CONTINUE = 100;

/* Get Client address. */
extern void* get_client_addr(struct sockaddr* ptr_client_addr) {
  return (ptr_client_addr->sa_family == AF_INET)
             ? (void*)&((struct sockaddr_in*)ptr_client_addr)->sin_addr    /* IPv4 addr. */
             : (void*)&((struct sockaddr_in6*)ptr_client_addr)->sin6_addr; /* Ipv6 addr. */
}

extern int set_server_protocols(struct addrinfo* ptr_server_info) {
  void* ret_val = memset(ptr_server_info, 0, sizeof(struct addrinfo));
  if (ret_val == NULL) {
    return MEMSET_FAILED;
  }
  ptr_server_info->ai_family = AF_INET6;      /* Use IPv6. */
  ptr_server_info->ai_socktype = SOCK_STREAM; /* Use TCP. */
  ptr_server_info->ai_flags = AI_PASSIVE;     /* The OS defines our IP address. */
  return EXIT_SUCCESS;
}

static struct addrinfo* set_getaddrinfo(struct addrinfo* ptr_server_info) {
  const char* SERVER_PORT = "65432";
  struct addrinfo* addr_info = (struct addrinfo*)malloc(sizeof(struct addrinfo)); /* TODO: FREE THIS. */
  /* getaddrinfo() will return information on our host and load up the struct sockaddr. */
  int return_value = getaddrinfo(NULL, SERVER_PORT, ptr_server_info, &addr_info);
  if (return_value != 0) {
    error_get_addr_info(return_value); /* gai_strerror(), get error message string from EAI_xxx error code. */
    return NULL;
  }
  return addr_info;
}

static int* set_socket(struct addrinfo* ptr_addr_info, int* server_file_descriptor) {
  *server_file_descriptor = socket(ptr_addr_info->ai_family, ptr_addr_info->ai_socktype, ptr_addr_info->ai_protocol);
  if (*server_file_descriptor == -1) {
    error_set_socket();
    perror("Error: SERVER -> socket(): "); /* Prints errno error message. */
    return NULL;
  }
  return server_file_descriptor;
}

static int set_setsockopt(const int* server_file_descriptor) {
  /*
   * Socket configuration.
   * SOL_SOCKET   - manipulate the socket-level options;
   * SO_REUSEADDR - Allows other sockets to bind() to this port, unless
   *                there is an active listening socket bound to the port
   *                already. This enables us to get around those
   *                “Address already in use” error messages when we try
   *                to restart our server after a crash.
   */
  const unsigned int YES = 1;
  int return_value = setsockopt(*server_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(int));
  if (return_value == -1) {
    error_set_sock_opt();
    perror("Error: SERVER -> setsockopt(): "); /* Prints errno error message. */
    /*
     * End the program we were unable to do a successful socket configuration, it doesn't matter if we change the
     * server_file_descriptor.
     */
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

extern int close_communications(const int* file_descriptor) {
  int return_value = close(*file_descriptor);
  if (return_value == -1) {
    error_close();
    perror(""); /* Prints errno error message. */
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static int set_bind(const int* server_file_descriptor, struct addrinfo* addr_info) {
  /*
   * Associate server_file_descriptor with an IP address (given by the OS) and a port number (defined by SERVER_PORT).
   */
  int return_value = bind(*server_file_descriptor, addr_info->ai_addr, addr_info->ai_addrlen);
  if (return_value == -1) {
    error_set_bind();
    perror(""); /* Prints errno error message. */
    return_value = close_communications(server_file_descriptor);
    if (return_value == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
    return CONTINUE;
  }
  return EXIT_SUCCESS;
}

extern int* set_server_file_descriptor(struct addrinfo* ptr_server_info) {
  struct addrinfo* addr_info = set_getaddrinfo(ptr_server_info);
  if (addr_info == NULL) {
    return NULL;
  }
  int* server_file_descriptor = (int*)malloc(sizeof(int));
  struct addrinfo* ptr_addr_info = addr_info;
  /* Loop through all the results and bind to the first we can. */
  for (; ptr_addr_info != NULL; ptr_addr_info = ptr_addr_info->ai_next) {
    /* Create an endpoint for communication and return a file descriptor. */
    server_file_descriptor = set_socket(ptr_addr_info, server_file_descriptor);
    if (server_file_descriptor == NULL) {
      continue;
    }
    /* Socket configuration. */
    int return_value = set_setsockopt(server_file_descriptor);
    if (return_value == EXIT_FAILURE) {
      return NULL;
    }
    /*
     * Associate server_file_descriptor with an IP address (given by the OS) and a port number (defined by SERVER_PORT).
     */
    return_value = set_bind(server_file_descriptor, addr_info);
    if (return_value == EXIT_FAILURE) {
      return NULL;
    }
    if (return_value == CONTINUE) {
      continue;
    }
    break;
  }

  /* Reassure if the socket in bond. */
  if (ptr_addr_info == NULL) {
    error_addr_info();
    return NULL;
  }

  /* Free allocated resource. */
  freeaddrinfo(addr_info);

  return server_file_descriptor;
}
