#include "embsys_lab1_socket.h"

#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO: mention in the @brief description of the functions the cases were we free and close file descriptors/and other
// TODO: memory.
// TODO: before submitting review all documentation in .c and .h files.

/*****************************************************************************/
/*************************** Private  Elements *******************************/
/*****************************************************************************/

/*************************** Variables ***************************************/
/**
 * @brief Error code returned by the set_server_bind() function when it fails. The function fails when the server file
 * descriptor is already in use. In this case, we must keep searching for a available file descriptor.
 */
static const int BIND_ERROR_CONTINUE = -100;

/*************************** Functions ***************************************/

/**
 * @brief The server protocols will be set to use IPv6 and TCP/IP, the OS defines the IP address. If the function fails,
 * returns EXIT_FAILURE, otherwise returns EXIT_SUCCESS. This function is used by the function
 * get_server_file_descriptor().
 *
 * @param protocols The server protocols. This is a pointer to a structure of type struct addrinfo.
 * @return int EXIT_FAILURE if the function fails, otherwise returns EXIT_SUCCESS.
 */
static int set_server_protocols(struct addrinfo* protocols) {
  if (memset(protocols, 0, sizeof(struct addrinfo)) == NULL) {  // Clear the structure.
    (void)stderr_print("Error: SERVER -> set_server_protocol() : memset() failed ");
    return EXIT_FAILURE;
  }
  protocols->ai_family = AF_INET6;       // IP version 6.
  protocols->ai_socktype = SOCK_STREAM;  // TCP/IP.
  protocols->ai_flags = AI_PASSIVE;      // The OS defines the IP address.
  return EXIT_SUCCESS;
}

/**
 * @brief This will give the server a set of protocols and a port number. If the getaddrinfo() fails, returns
 * EXIT_FAILURE. Otherwise, the function returns EXIT_SUCCESS. This function is called by the
 * get_server_file_descriptor() function.
 *
 * @param protocols The server protocols. This is a pointer to a structure of type struct addrinfo.
 * @param information The server information. This is a pointer to a pointer, to a structure of type struct addrinfo.
 * @return int EXIT_FAILURE if the function fails, otherwise returns EXIT_SUCCESS.
 */
static int set_server_addr_info(struct addrinfo* protocols, struct addrinfo** information) {
  // getaddrinfo() will return information on our host and load up addrinfo** information.
  int return_addrinfo = getaddrinfo(NULL, SERVER_PORT, protocols, information);
  if (return_addrinfo != 0) {
    // gai_strerror(), prints the error message string from the EAI_xxx error code.
    int return_print = fprintf(stderr, "Error: SERVER -> getaddrinfo() : %s\n", gai_strerror(return_addrinfo));
    if (return_print < 0) {
      perror("Error: SERVER -> getaddrinfo() -> fprintf() ");
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Set the server socket options. This function is used to set the SO_REUSEADDR option. This option allows
 * other sockets to bind() to the server port, unless the server socket is already bound to the port and listening. This
 * enables us to get around those "Address already in use" error messages when we try to restart our server after a
 * crash. This function is called by the get_server_file_descriptor() function. If the function fails, the program
 * must end, it doesn't matter if we change the server file descriptor. In success the server file descriptor associated
 * information is changed.
 *
 * @param server file descriptor. This is a pointer to a structure of type file_descriptor.
 * @return int EXIT_SUCCESS if it was successful, EXIT_FAILURE otherwise.
 */
static int set_server_socket_options(const file_descriptor* server) {
  const unsigned int YES = 1;
  // SOL_SOCKET - manipulate the socket-level options.
  if (setsockopt(*server, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(int)) == -1) {
    perror("Error: SERVER -> setsockopt() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief This function will bind the server file descriptor to the IP address and port number. If the function fails,
 * returns BIND_ERROR_CONTINUE, otherwise returns EXIT_SUCCESS. This function is called by the
 * get_server_file_descriptor() function.
 *
 * @param server file descriptor. This is a pointer to a structure of type file_descriptor.
 * @param information the server information. This is a pointer to a structure of type struct addrinfo.
 * @return int BIND_ERROR_CONTINUE if the function fails, otherwise returns EXIT_SUCCESS.
 */
static int set_server_bind(const file_descriptor* server, struct addrinfo* information) {
  // Associate the server file descriptor with an IP address (given by the OS) and a port number (defined by
  // SERVER_PORT).
  if (bind(*server, information->ai_addr, information->ai_addrlen) == -1) {
    perror("Error: SERVER -> bind() ");
    return BIND_ERROR_CONTINUE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Returns the client IP address (v.4 or v.6). This function is called by the accept_incoming_connection()
 * function.
 *
 * @param client_address client address information. This is a pointer to a structure of type struct sockaddr.
 * @return void* client IP address.
 */
static void* get_client_ip_addr(struct sockaddr* client_address) {
  return (client_address->sa_family == AF_INET)
             ? (void*)&((struct sockaddr_in*)client_address)->sin_addr     // IP version 4 address.
             : (void*)&((struct sockaddr_in6*)client_address)->sin6_addr;  // IP version 6 address.
}

/*****************************************************************************/
/*************************** Public Elements *********************************/
/*****************************************************************************/

/*************************** Variables ***************************************/
/**
 * @brief The server port to connect to.
 */
const char* const SERVER_PORT = "65432";

/**
 * @brief The SERVER_BACKLOG is the maximum number of connections that the kernel should queue for this socket. The
 * maximum value for this parameter is SOMAXCONN. The minimum value is 1. The default value in this implementation is
 * 128.
 */
const int SERVER_BACKLOG = 128;

/**
 * @brief  This is the maximum number of bytes that can be received from a client.
 */
const unsigned int MAX_CLIENT_DATA = 100;

/**
 * @brief The server must keep listening for incoming connections.§
 */
const int SOCKET_CONTINUE_LISTENING = -101;

/*************************** Functions ***************************************/
/**
 * @brief This function will create a server file descriptor. If the function fails, returns EXIT_FAILURE, otherwise
 * returns EXIT_SUCCESS.
 *
 * @param server file descriptor. This is a pointer to a structure of type file_descriptor.
 * @return int EXIT_FAILURE if the function fails, otherwise returns EXIT_SUCCESS.
 */
extern int get_server_file_descriptor(file_descriptor* server) {
  // This defines the IP version, Transport Layer and IP address that the server is going to use.
  struct addrinfo protocols;
  if (set_server_protocols(&protocols) == EXIT_FAILURE) {
    free(server);
    return EXIT_FAILURE;
  }
  struct addrinfo* information = NULL;  // Define the server protocols and port number.
  if (set_server_addr_info(&protocols, &information) == EXIT_FAILURE) {
    free(server);
    return EXIT_FAILURE;
  }
  // The "addrinfo* information" is a linked list of structures. Each structure contains the address information for one
  // of the network interfaces on the host machine. The loop will go through all the interfaces and try to
  // set_server_bind() to the first one that works. If the set_server_bind() succeeds, the loop will break. If the loop
  // finishes, the set_server_bind() failed on all interfaces.
  for (; information != NULL; information = information->ai_next) {
    // socket() creates an endpoint for communication and returns a file descriptor that refers to that endpoint.
    *server = socket(information->ai_family, information->ai_socktype, information->ai_protocol);
    if (*server == -1) {
      perror("Error: SERVER -> get_server_file_descriptor() -> socket() ");
      continue;
    }
    // Set the server socket options (SO_REUSEADDR).
    if (set_server_socket_options(server) == EXIT_FAILURE) {
      (void)stderr_print("Error: SERVER -> get_server_file_descriptor() -> set_server_socket_options() ");
      free(server);
      return EXIT_FAILURE;
    }
    // Bind the server file descriptor to the server IP address and port number.
    if (set_server_bind(server, information) == BIND_ERROR_CONTINUE) {
      if (close(*server) == -1) {
        perror("Error: SERVER -> get_server_file_descriptor() -> close() ");
        free(server);
        return EXIT_FAILURE;
      }
      continue;
    }
    break;  // The file descriptor was found.
  }
  // If the "information" is NULL, the set_server_bind() failed on all interfaces.
  if (information == NULL) {
    (void)stderr_print("Error: SERVER -> get_server_file_descriptor() -> set_server_bind() : (information == NULL) ");
    free(server);
    return EXIT_FAILURE;
  }
  // Free allocated resource.
  freeaddrinfo(information);
  return EXIT_SUCCESS;
}

/**
 * @brief This function will set the server to listen for incoming connections. The SERVER_BACKLOG parameter defines
 + the maximum length to which the queue of pending connections may grow. If the function fails, returns EXIT_FAILURE,
 * otherwise returns EXIT_SUCCESS.
 *
 * @param server file descriptor. This is a pointer to a structure of type file_descriptor.
 * @return int EXIT_FAILURE if the function fails, otherwise returns EXIT_SUCCESS.
 */
extern int set_server_listen(file_descriptor* server) {
  if (listen(*server, SERVER_BACKLOG) == -1) {
    perror("Error: SERVER -> listen() ");
    (void)close_socket(server);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Close the socket connection. And free the file descriptor malloced memory.
 *
 * @param open_socket file descriptor. This is a pointer to a structure of type file_descriptor.
 * @return int EXIT_FAILURE if the function fails, otherwise returns EXIT_SUCCESS.
 */
extern int close_socket(file_descriptor* open_socket) {
  bool error = false;
  if (close(*open_socket) == -1) {
    perror("Error: SERVER -> close_socket() -> close() ");
    error = true;
  }
  free(open_socket);
  return error ? EXIT_FAILURE : EXIT_SUCCESS;
}

/**
 * @brief Accept a socket connection and therefore set the file descriptor and IP address of a client. If the function
 * fails to get the client file descriptor returns SOCKET_CONTINUE_LISTENING, if a system error occurs returns
 * EXIT_FAILURE, otherwise returns EXIT_SUCCESS. If the function fails, the client file descriptor will be closed and
 * freed and the client_ip_address will be freed, if the failure happens because of the system the server will also be
 * closed and freed. If the function succeeds, the client file descriptor and IP address will be known.
 *
 * @param server file descriptor. This is a pointer to a structure of type file_descriptor.
 * @param client file descriptor. This is a pointer to a structure of type file_descriptor.
 * @param client_ip_address client IP address. This is a pointer of type char.
 * @return int SOCKET_CONTINUE_LISTENING if the function fails to get the file descriptor, EXIT_FAILURE if a system
 * error occurs, otherwise returns EXIT_SUCCESS.
 */
extern int accept_incoming_connection(file_descriptor* server, file_descriptor* client, char* client_ip_address) {
  bool exit_failure = false;
  struct sockaddr_storage client_address;
  socklen_t client_size = sizeof client_address;
  // Accept an incoming connection.
  *client = accept(*server, (struct sockaddr*)&client_address, &client_size);
  if (*client == -1) {
    perror("Error: SERVER -> accept_incoming_connection() -> accept() ");
    if (close_socket(client) == EXIT_FAILURE) {
      (void)close_socket(server);
      exit_failure = true;
    }
    return exit_failure ? EXIT_FAILURE : SOCKET_CONTINUE_LISTENING;
  }
  // Get the client IP address, in binary form (network byte order).
  void* _clnt_ip_addr = get_client_ip_addr((struct sockaddr*)&client_address);
  // Transform the IP address to a presentation format ("human readable").
  if (inet_ntop(client_address.ss_family, _clnt_ip_addr, client_ip_address, sizeof client_ip_address) ==
      NULL) {  // inet_ntop() failed, it's a system error, close server.
    perror("Error: SERVER -> accept_incoming_connection() -> inet_ntop() ");
    (void)close_socket(client);
    (void)close_socket(server);
    return EXIT_FAILURE;
  }
  int return_value =
      printf("Receive a connection from : %s\n\t ( its file descriptor : %d )\n", client_ip_address, *client);
  if (return_value < 0) {  // printf() failed, it's a system error, close server.
    perror("Error: SERVER -> accept_incoming_connection() -> printf() ");
    (void)close_socket(client);
    (void)close_socket(server);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief This function will receive data from a client. If the function fails to receive data returns
 * SOCKET_CONTINUE_LISTENING, if a system error occurs returns EXIT_FAILURE, otherwise returns EXIT_SUCCESS. If the
 * function fails, the client file descriptor will be closed and freed, if the failure happens because of the system
 * the server will also be closed and freed. If the function succeeds, data send by the client will be known.
 *
 * @param server file descriptor.
 * @param client_ip_address client IP address.
 * @param client file descriptor.
 * @param client_data client data.
 * @return int SOCKET_CONTINUE_LISTENING if the function fails to receive data, EXIT_FAILURE if a system error occurs,
 * otherwise returns EXIT_SUCCESS.
 */
extern int receive_client_data(file_descriptor* server, const char* client_ip_address, file_descriptor* client,
                               char* client_data) {
  bool exit_failure = false;
  // The maximum number of bytes that can be received is MAX_CLIENT_DATA - 1. The last byte is reserved for the null
  // terminator. The recv() function returns the number of bytes received. If the number of bytes received is -1, the
  // recv() function failed. If the number of bytes received is 0, the client has closed the connection.
  ssize_t number_bytes = recv(*client, client_data, MAX_CLIENT_DATA - 1, 0);
  if (number_bytes == -1 || number_bytes == 0) {
    char* recv_failed =
        number_bytes < 0 ? "Error: SERVER -> receive_client_data() -> recv() failed recv() failed "
                         : "Error: SERVER -> receive_client_data() -> recv() failed client has closed the connection ";

    if (stderr_print(recv_failed) == EXIT_FAILURE) {
      exit_failure = true;
    }
    int return_value = fprintf(stderr,
                               "Error: SERVER -> receive_client_data() -> unable to receive data from -> %s\n\tClosing "
                               "communication with client",
                               client_ip_address);
    if (return_value < 0) {
      exit_failure = true;
    }
    if (close_socket(client) == EXIT_FAILURE) {
      exit_failure = true;
    }
    if (exit_failure) {
      (void)close_socket(server);
      return EXIT_FAILURE;
    }
    return SOCKET_CONTINUE_LISTENING;
  }
  // Add the null terminator to the end of the string.
  client_data[number_bytes] = '\0';
  return EXIT_SUCCESS;
}

/**
 * @brief This function will send data to a client. If the function fails to send data returns EXIT_FAILURE, otherwise
 * returns EXIT_SUCCESS. If the function fails, the client and server file descriptor will be closed and freed.
 *
 * @param server file descriptor.
 * @param client_ip_address client IP address.
 * @param client file descriptor.
 * @param data_to_client data to send to client.
 * @return int EXIT_FAILURE if the function fails to send data, otherwise returns EXIT_SUCCESS.
 */
extern int send_data_to_client(file_descriptor* server, const char* client_ip_address, file_descriptor* client,
                               const char* data_to_client) {
  // The send() function returns the number of bytes sent. If the number of bytes sent is -1, the send() function
  // failed. If the number of bytes sent is 0, the client has closed the connection. The send() function sends data only
  // to a socket in a connected state.
  ssize_t return_value = send(*client, data_to_client, strlen(data_to_client), 0);
  bool exit_failure = false;
  char* error_str = NULL;
  if (return_value == -1 || return_value == 0) {
    error_str = return_value == -1
                    ? "Error: SERVER -> send_data_to_client() -> send() failed "
                    : "Error: SERVER -> send_data_to_client() -> send() client has closed the connection ";
    (void)stderr_print(error_str);
    exit_failure = true;
  }
  if (exit_failure) {
    int return_value =
        fprintf(stderr, "Error: SERVER -> unable to send data to -> %s\n\tClosing communication with client",
                client_ip_address);
    if (return_value < 0) {
      perror("Error: SERVER -> send_data_to_client() : fprintf() ");
    }
    (void)close_socket(client);
    (void)close_socket(server);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
