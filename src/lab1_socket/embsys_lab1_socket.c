#include "embsys_lab1_socket.h"

#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************/
/*************************** Private  Elements *******************************/
/*****************************************************************************/

static const int BIND_ERROR_CONTINUE = -100;

/* Returns the client IP address ( IP v.4 or v.6 ). */
static void* get_client_ip_addr(struct sockaddr* client_addr) {
  return (client_addr->sa_family == AF_INET)
             ? (void*)&((struct sockaddr_in*)client_addr)->sin_addr    /* IP version 4 address. */
             : (void*)&((struct sockaddr_in6*)client_addr)->sin6_addr; /* IP version 6 address. */
}

/* Bind the Server file descriptor to the Server IP address. */
static int set_bind(const file_descriptor* server, struct addrinfo* server_info) {
  /*
   * Associate a file descriptor with an IP address (given by the OS)
   * and a port number (defined by SERVER_PORT) to our Server.
   */
  if (bind(*server, server_info->ai_addr, server_info->ai_addrlen) == -1) {
    perror("Error: SERVER -> bind() ");
    /* Close open Socket with this file descriptor and delete it. */
    if (close_communication(server) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
    return BIND_ERROR_CONTINUE;
  }
  return EXIT_SUCCESS;
}

/* Create an endpoint for the Server to be able to communicate. */
static int set_server_socket(file_descriptor* server, struct addrinfo* sever_info) {
  /* Create an endpoint for communication and get a file descriptor. */
  *server = socket(sever_info->ai_family, sever_info->ai_socktype, sever_info->ai_protocol);
  if (*server == -1) {
    perror("Error: SERVER -> socket() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/* Establish that this Server is SO_REUSEADDR. */
static int set_server_socket_options(const file_descriptor* server) {
  const unsigned int YES = 1;
  /*
   * Socket configuration.
   * SOL_SOCKET   - manipulate the socket-level options;
   * SO_REUSEADDR - Allows other sockets to bind() to this port, unless
   *                there is an active listening socket bound to the port
   *                already. This enables us to get around those
   *                "Address already in use" error messages when we try
   *                to restart our server after a crash.
   */
  if (setsockopt(*server, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(int)) == -1) {
    perror("Error: SERVER -> setsockopt() ");
    /*
     * End the program. We were unable to do a successful socket configuration, it doesn't matter if we change the
     * Server file descriptor.
     */
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************/
/*************************** Public Elements *********************************/
/*****************************************************************************/

/* Define the Internet Protocols that the Server will use. */
extern int set_server_protocol(struct addrinfo* protocols) {
  if (memset(protocols, 0, sizeof(struct addrinfo)) == NULL) {
    (void)stderr_print("Error: SERVER -> set_server_protocol() : memset() failed ");
    return EXIT_FAILURE;
  }
  protocols->ai_family = AF_INET6;      /* Use IP version 6. */
  protocols->ai_socktype = SOCK_STREAM; /* Use TC P/IP. */
  protocols->ai_flags = AI_PASSIVE;     /* The OS defines our IP. */
  return EXIT_SUCCESS;
}

/* Set the Server address structure and service name. */
extern int set_server_addr_info(struct addrinfo* protocols, struct addrinfo** server_info) {
  /* getaddrinfo() will return information on our host and load up the struct sockaddr. */
  const char* SERVER_PORT = "65432";
  if (getaddrinfo(NULL, SERVER_PORT, protocols, server_info) != 0) {
    /* gai_strerror(), prints error message string from the EAI_xxx error code. */
    int return_value = fprintf(stderr, "Error: SERVER -> getaddrinfo() : %s\n", gai_strerror(return_value));
    if (return_value < 0) {
      perror("Error: SERVER -> getaddrinfo() : fprintf() ");
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/* Close communications and delete the file descriptor. */
extern int close_communication(const file_descriptor* open_socket) {
  if (close(*open_socket) == -1) {
    perror("Error: SERVER -> close() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/* Find a file descriptor for Server. */
extern int get_server_file_descriptor(file_descriptor* server, struct addrinfo* server_info) {
  struct addrinfo* ptr_server_info = server_info;
  /* Loop through all the results and bind to the first we can. */
  for (; ptr_server_info != NULL; ptr_server_info = ptr_server_info->ai_next) {
    /* Create an endpoint for the Server to be able to communicate. */
    if (set_server_socket(server, server_info) == EXIT_FAILURE) {
      continue;
    }
    /* Establish that this Server is SO_REUSEADDR. */
    if (set_server_socket_options(server) == EXIT_FAILURE) {
      (void)stderr_print("Error: SERVER -> get_server_file_descriptor() ");
      return EXIT_FAILURE;
    }
    /* Bind the Server file descriptor to the Server IP address. */
    int return_value = set_bind(server, ptr_server_info);
    if (return_value == BIND_ERROR_CONTINUE) {
      continue;
    }
    if (return_value == EXIT_FAILURE) {
      (void)stderr_print("Error: SERVER -> get_server_file_descriptor() ");
      return EXIT_FAILURE;
    }
    break;
  }
  /* Free allocated resource. */
  freeaddrinfo(server_info);
  /* Reassure if the socket is bond. */
  if (ptr_server_info == NULL) {
    (void)stderr_print("Error: SERVER -> bind() : (ptr_server_info == NULL) ");
    (void)stderr_print("Error: SERVER -> get_server_file_descriptor() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/* Listen for connections on Server socket. */
extern int set_listen(const file_descriptor* server) {
  /* How many pending connections the system queue will hold. */
  const int SERVER_BACKLOG = 128;
  /*  Listen for incoming connections. */
  if (listen(*server, SERVER_BACKLOG) == -1) {
    perror("Error: SERVER -> listen() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/* Accept a socket connection and therefore set the file descriptor and IP address of a Client. */
extern int set_client(const file_descriptor* server, file_descriptor* client, char* client_ip_addr) {
  struct sockaddr_storage client_address;
  socklen_t client_size = sizeof client_address;
  /* Accept an incoming connection. */
  *client = accept(*server, (struct sockaddr*)&client_address, &client_size);
  if (*client == -1) {
    perror("Error: SERVER -> accept() ");
    return SOCKET_CONTINUE_LISTENING;
  }
  /* Get the Client IP address. */
  void* client_ip_address = get_client_ip_addr((struct sockaddr*)&client_address);
  /*
   * Transform the IP address that is stored in binary form (network byte order)
   * to a presentation format ("human readable").
   */
  const char* return_string =
      inet_ntop(client_address.ss_family, client_ip_address, client_ip_addr, sizeof client_ip_addr);
  if (return_string == NULL) {
    perror("Error: SERVER -> inet_ntop() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/* Get data from the Client. */
extern int get_client_data(const file_descriptor* client, char* client_data) {
  /* Receive data only from a connected socket. */
  ssize_t number_bytes = recv(*client, client_data, MAX_CLIENT_DATA - 1, 0);
  if (number_bytes == -1) {
    perror("Error: SERVER -> recv() ");
    return EXIT_FAILURE;
  }
  client_data[number_bytes] = '\0';
  return EXIT_SUCCESS;
}

/* Send data to a Client. */
extern int send_data_to_client(const file_descriptor* client, const char* data_to_send) {
  /* Send data only to a socket in a connected state. */
  ssize_t return_value = send(*client, data_to_send, strlen(data_to_send), 0);
  if (return_value == -1) {
    perror("Error: SERVER -> send() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
