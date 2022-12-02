#include "embsys_lab1_socket.h"

#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Get Client address. */
extern void* get_client_addr(struct sockaddr* ptr_client_addr) {
  return (ptr_client_addr->sa_family == AF_INET)
             ? (void*)&((struct sockaddr_in*)ptr_client_addr)->sin_addr    /* IPv4 addr. */
             : (void*)&((struct sockaddr_in6*)ptr_client_addr)->sin6_addr; /* Ipv6 addr. */
}

extern int set_server_protocol(struct addrinfo* ptr_server_protocols) {
  void* ptr = memset(ptr_server_protocols, 0, sizeof(struct addrinfo));
  ptr_server_protocols->ai_family = AF_INET6;      /* Use IPv6. */
  ptr_server_protocols->ai_socktype = SOCK_STREAM; /* Use TCP. */
  ptr_server_protocols->ai_flags = AI_PASSIVE;     /* The OS defines our IP address. */
  if (ptr == NULL) {
    if (fprintf(stderr, "Error: SERVER -> set_server_protocol() : memset() failed.\n") < 0) {
      perror("Error: SERVER -> set_server_protocol() : memset() : fprintf()\n");
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

extern int set_server_addr_info(struct addrinfo* ptr_server_protocols, struct addrinfo** ptr_server_info) {
  /* getaddrinfo() will return information on our host and load up the struct sockaddr. */
  const char* SERVER_PORT = "65432";
  int return_value = getaddrinfo(NULL, SERVER_PORT, ptr_server_protocols, ptr_server_info);
  if (return_value != 0) {
    /* gai_strerror(), get error message string from EAI_xxx error code. */
    if (fprintf(stderr, "Error: SERVER -> getaddrinfo() : %s\n", gai_strerror(return_value)) < 0) {
      perror("Error: SERVER -> getaddrinfo()() : fprintf()\n");
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static int set_socket(int* server_file_descriptor, struct addrinfo* ptr_server_info) {
  /* Create an endpoint for communication and get a file descriptor. */
  *server_file_descriptor =
      socket(ptr_server_info->ai_family, ptr_server_info->ai_socktype, ptr_server_info->ai_protocol);
  if (*server_file_descriptor == -1) {
    perror("Error: SERVER -> socket() : ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static int set_socket_opt(const int* server_file_descriptor) {
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
  if (setsockopt(*server_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(int)) == -1) {
    /* Prints errno error message. */
    perror("Error: SERVER -> setsockopt() : ");
    /* End the program we were unable to do a successful socket
     * configuration, it doesn't matter if we change the socket_fd. */
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

extern int close_communication(const int* file_descriptor) {
  if (close(*file_descriptor) == -1) {
    perror("Error: SERVER -> close() : ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
static const int BIND_ERROR_CONTINUE = -100;
static int set_bind(const int* server_files_descriptor, struct addrinfo* ptr_server_info) {
  /* Associate socket_fd with an IP address (given by the OS) and
   * a port number (defined by SERVER_PORT). */
  if (bind(*server_files_descriptor, ptr_server_info->ai_addr, ptr_server_info->ai_addrlen) == -1) {
    perror("Error: SERVER -> bind() : ");
    if (close_communication(server_files_descriptor) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
    return BIND_ERROR_CONTINUE;
  }
  return EXIT_SUCCESS;
}

extern int get_server_file_descriptor(int* server_file_descriptor, struct addrinfo* server_info) {
  struct addrinfo* ptr_server_info = server_info;
  /* Loop through all the results and bind to the first we can. */
  for (; ptr_server_info != NULL; ptr_server_info = ptr_server_info->ai_next) {
    if (set_socket(server_file_descriptor, server_info) == EXIT_FAILURE) {
      continue;
    }
    if (set_socket_opt(server_file_descriptor) == EXIT_FAILURE) {
      if (fprintf(stderr, "Error: SERVER -> get_server_file_descriptor().\n") < 0) {
        perror("Error: SERVER -> get_server_file_descriptor() : fprintf() : \n");
      }
      return EXIT_FAILURE;
    }
    int return_value = set_bind(server_file_descriptor, ptr_server_info);
    if (return_value == BIND_ERROR_CONTINUE) {
      continue;
    }
    if (return_value == EXIT_FAILURE) {
      if (fprintf(stderr, "Error: SERVER -> get_server_file_descriptor().\n") < 0) {
        perror("Error: SERVER -> get_server_file_descriptor() : fprintf() : \n");
      }
      return EXIT_FAILURE;
    }
    break;
  }
  /* Free allocated resource. */
  freeaddrinfo(server_info);
  /* Reassure if the socket in bond. */
  if (ptr_server_info == NULL) {
    if (fprintf(stderr, "Error: SERVER -> bind(): (ptr_server_info == NULL)\n") < 0) {
      perror("Error: SERVER -> bind() : fprintf() : \n");
    }
    if (fprintf(stderr, "Error: SERVER -> get_server_file_descriptor().\n") < 0) {
      perror("Error: SERVER -> bind() : get_server_file_descriptor() : fprintf() \n");
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

extern int set_listen(const int* server_file_descriptor) {
  /* How many pending connections the system queue will hold. */
  const int SERVER_BACKLOG = 128;
  /*  Listen for incoming connections. */
  if (listen(*server_file_descriptor, SERVER_BACKLOG) == -1) {
    perror("Error: SERVER -> listen(): "); /* Prints errno error message. */
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

extern int set_client_file_descriptor(const int* server_file_descriptor, int* client_file_descriptor,
                                      char* client_ip_addr) {
  struct sockaddr_storage client_addr;
  socklen_t client_size = sizeof client_addr;
  *client_file_descriptor = accept(*server_file_descriptor, (struct sockaddr*)&client_addr, &client_size);
  if (*client_file_descriptor == -1) {
    /* Prints errno error message. */
    perror("Error: SERVER -> accept(): ");
    return CONTINUE;
  }
  /* Get Client IP address. */
  void* ip_addr = get_client_addr((struct sockaddr*)&client_addr);
  /*
   * Transform IP address that is stored in some binary form (in network byte order) to a presentation format ("human
   * readable").
   */
  if (inet_ntop(client_addr.ss_family, ip_addr, client_ip_addr, sizeof client_ip_addr) == NULL) {
    /* Prints errno error message. */
    perror("Error: SERVER -> inet_ntop(): ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

extern int get_client_data(const int* client_file_descriptor, char* client_data) {
  /* Get data from Client. */
  /* Receive data only from a connected socket. */
  ssize_t number_bytes = recv(*client_file_descriptor, client_data, MAX_CLIENT_DATA - 1, 0);
  if (number_bytes == -1) {
    perror("Error: SERVER -> recv(): ");
    close(*client_file_descriptor);
    return EXIT_FAILURE;
  }
  client_data[number_bytes] = '\0';
  return EXIT_SUCCESS;
}
