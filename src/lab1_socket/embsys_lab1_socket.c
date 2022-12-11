#include "embsys_lab1_socket.h"

#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************/
/*************************** Private  Elements *******************************/
/*****************************************************************************/

/*************************** Variables **************************************/

/**
 * @brief Error code returned by the set_bind() function when the bind() system call fails. The bind() system call
 * fails when the server file descriptor is already in use. In this case, we must keep searching for a available
 * file descriptor.
 */
static const int BIND_ERROR_CONTINUE = -100;

/*************************** Functions ********************************/

/**
 * @brief Returns the client IP address (v.4 or v.6). This function is called by the set_client() function.
 *
 * @param client_address client address information.
 * @return void* client IP address.
 */
static void* get_client_ip_addr(struct sockaddr* client_address) {
  return (client_address->sa_family == AF_INET)
             ? (void*)&((struct sockaddr_in*)client_address)->sin_addr     // IP version 4 address.
             : (void*)&((struct sockaddr_in6*)client_address)->sin6_addr;  // IP version 6 address.
}

/**
 * @brief Bind the server file descriptor to the server IP address and port number. If the bind() fails, close the
 * communication and return the BIND_ERROR_CONTINUE error code. If the close_communication() fails, return the
 * EXIT_FAILURE error code. Otherwise, return EXIT_SUCCESS. This function is called by the get_server_file_descriptor()
 * function.
 *
 * @param server file descriptor.
 * @param server_info server address and port information.
 * @return int EXIT_SUCCESS if the bind() was successful, EXIT_FAILURE if close_communication() failed,
 * BIND_ERROR_CONTINUE otherwise.
 */
static int set_bind(const file_descriptor* server, struct addrinfo* server_info) {
  // Associate the server file descriptor with an IP address (given by the OS) and a port number (defined by
  // SERVER_PORT).
  if (bind(*server, server_info->ai_addr, server_info->ai_addrlen) == -1) {
    perror("Error: SERVER -> bind() ");
    if (close_communication(server) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
    return BIND_ERROR_CONTINUE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Create an endpoint for the server to be able to communicate. The server file descriptor is set to the
 * socket() return value. If the socket() fails, set_server_socket() returns EXIT_FAILURE, otherwise returns
 * EXIT_SUCCESS. The set_server_socket() function is called by the get_server_file_descriptor() function.
 *
 * @param server  file descriptor.
 * @param sever_info  server address and port information.
 * @return int  EXIT_SUCCESS if the socket() was successful, EXIT_FAILURE otherwise.
 */
static int set_server_socket(file_descriptor* server, struct addrinfo* sever_info) {
  *server = socket(sever_info->ai_family, sever_info->ai_socktype, sever_info->ai_protocol);
  if (*server == -1) {
    perror("Error: SERVER -> socket() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Set the server socket options. This function is used to set the SO_REUSEADDR option. This option allows
 * other sockets to bind() to the server port, unless the server socket is already bound to the port and listening. This
 * enables us to get around those "Address already in use" error messages when we try to restart our server after a
 * crash. This function is called by the get_server_file_descriptor() function. If the setsockopt() fails, the program
 * is ended, it doesn't matter if we change the server file descriptor. If the setsockopt() is successful, the server
 * file descriptor associated information is changed.
 *
 * @param server file descriptor. The file descriptor associated information is changed if the setsockopt() is
 * successful.
 * @return int EXIT_SUCCESS if the setsockopt() was successful, EXIT_FAILURE otherwise.
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
 * @brief The server protocols will be set to IPv6 and TCP/IP, and the OS defines the IP address. If the function fails,
 * the program is ended. This function is called by the get_server_file_descriptor() function.
 *
 * @param protocols server protocols. The server protocols are set to IPv6 and TCP/IP, the OS defines the IP address.
 * @return int EXIT_SUCCESS if successful, EXIT_FAILURE otherwise.
 */
static int set_server_protocols(struct addrinfo* protocols) {
  if (memset(protocols, 0, sizeof(struct addrinfo)) == NULL) {
    (void)stderr_print("Error: SERVER -> set_server_protocol() : memset() failed ");
    return EXIT_FAILURE;
  }
  protocols->ai_family = AF_INET6;       // Ip version 6.
  protocols->ai_socktype = SOCK_STREAM;  // TCP/IP.
  protocols->ai_flags = AI_PASSIVE;      // The OS defines the IP address.
  return EXIT_SUCCESS;
}

/**
 * @brief This will give the server a set of protocols and a port number. The server address structure and service name
 * are set to the server_info parameter. If the getaddrinfo() fails, the program is ended. Otherwise, the function
 * returns EXIT_SUCCESS. This function is called by the get_server_file_descriptor() function.
 *
 * @param protocols server protocols.
 * @param server_info server where the address and port information will be stored.
 * @return int EXIT_SUCCESS if the getaddrinfo() was successful, EXIT_FAILURE otherwise.
 */
static int set_server_addr_info(struct addrinfo* protocols, struct addrinfo** server_info) {
  const char* SERVER_PORT = "65432";
  // getaddrinfo() will return information on our host and load up server_info.
  int return_addrinfo = getaddrinfo(NULL, SERVER_PORT, protocols, server_info);
  if (return_addrinfo != 0) {
    // gai_strerror(), prints the error message string from the EAI_xxx error code.
    int return_print = fprintf(stderr, "Error: SERVER -> getaddrinfo() : %s\n", gai_strerror(return_addrinfo));
    if (return_print < 0) {
      perror("Error: SERVER -> getaddrinfo() : fprintf() ");
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Accept a socket connection and therefore set the file descriptor and IP address of a client. If the accept()
 * fails, returns SOCKET_CONTINUE_LISTENING and the program should continue listening. Otherwise, the function returns
 * EXIT_SUCCESS. If it fails to get the client IP address, returns EXIT_FAILURE and the program should end.
 *
 * @param server file descriptor.
 * @param client file descriptor. The client will be able to send and receive messages.
 * @param client_ip_addr IP address of the client.
 * @return int EXIT_SUCCESS if the accept() was successful, SOCKET_CONTINUE_LISTENING otherwise. If it fails to get the
 * client IP address, returns EXIT_FAILURE.
 */
static int set_client(const file_descriptor* server, file_descriptor* client, char* client_ip_addr) {
  struct sockaddr_storage client_address;
  socklen_t client_size = sizeof client_address;
  // Accept an incoming connection.
  *client = accept(*server, (struct sockaddr*)&client_address, &client_size);
  if (*client == -1) {
    perror("Error: SERVER -> accept() ");
    return SOCKET_CONTINUE_LISTENING;
  }
  // Get the client IP address.
  void* client_ip_address = get_client_ip_addr((struct sockaddr*)&client_address);
  // Transform the IP address that is stored in binary form (network byte order) to a presentation format ("human
  // readable").
  const char* return_string =
      inet_ntop(client_address.ss_family, client_ip_address, client_ip_addr, sizeof client_ip_addr);
  if (return_string == NULL) {
    perror("Error: SERVER -> inet_ntop() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Get the client data. If the recv() fails, returns EXIT_FAILURE and the program should end.
 * Otherwise, the function returns EXIT_SUCCESS.
 *
 * @param client file descriptor.
 * @param client_data string that will store the client data.
 * @return int  EXIT_SUCCESS if the recv() was successful, EXIT_FAILURE otherwise.
 */
inline int get_client_data(const file_descriptor* client, char* client_data) {
  // The maximum number of bytes that can be received is MAX_CLIENT_DATA - 1. The last byte is reserved for the null
  // terminator. The recv() function returns the number of bytes received. If the number of bytes received is -1, the
  // recv() function failed. If the number of bytes received is 0, the client has closed the connection.
  ssize_t number_bytes = recv(*client, client_data, MAX_CLIENT_DATA - 1, 0);
  if (number_bytes == -1) {
    perror("Error: SERVER -> recv() failed ");
    return EXIT_FAILURE;
  }
  if (number_bytes == 0) {
    perror("Error: SERVER -> recv() closed connection ");
    return EXIT_FAILURE;
  }
  // Add the null terminator to the end of the string.
  client_data[number_bytes] = '\0';
  return EXIT_SUCCESS;
}

/*****************************************************************************/
/*************************** Public Elements *********************************/
/*****************************************************************************/

/**
 * @brief Close the communication and delete the file descriptor. If the close() fails, the program is ended.
 *
 * @param open_socket file descriptor. The file descriptor is deleted.
 * @return int EXIT_SUCCESS if the close() was successful, EXIT_FAILURE otherwise.
 */
extern int close_communication(const file_descriptor* open_socket) {
  if (close(*open_socket) == -1) {
    perror("Error: SERVER -> close() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
/**
 * @brief Close the socket connection. And free the file descriptor malloced memory.
 *
 * @param server file descriptor.
 */
extern void close_socket(file_descriptor* open_socket) {
  close_communication(open_socket);
  free(open_socket);
}

/**
 * @brief This function will find a file descriptor for the server. The server will be able to accept connections from
 * clients, send and receive messages and close communications. If the function fails, it returns a EXIT_FAILURE
 * and the program is ended. Otherwise, the function returns EXIT_SUCCESS.
 *
 * @param server file descriptor.
 * @return int EXIT_SUCCESS if the function was successful, EXIT_FAILURE otherwise.
 */
extern int get_server_file_descriptor(file_descriptor* server) {
  // This defines the IP version, Transport Layer and IP address that the server is going to use.
  struct addrinfo server_protocols;
  if (set_server_protocols(&server_protocols) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  // Define the server protocols and port number.
  struct addrinfo* server_info = NULL;
  if (set_server_addr_info(&server_protocols, &server_info) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  // The server_info is a linked list of structures. Each structure contains the address information for one of the
  // network interfaces on the host. The loop will go through all the interfaces and try to bind to the first one that
  // works. If the bind() fails, the loop will try the next interface. If the bind() succeeds, the loop will break.
  // If the loop finishes, the bind() failed on all interfaces.
  for (; server_info != NULL; server_info = server_info->ai_next) {
    // Create an endpoint for the server to be able to communicate.
    if (set_server_socket(server, server_info) == EXIT_FAILURE) {
      continue;
    }
    // Set the server socket options (SO_REUSEADDR).
    if (set_server_socket_options(server) == EXIT_FAILURE) {
      (void)stderr_print("Error: SERVER -> get_server_file_descriptor() ");
      return EXIT_FAILURE;
    }
    // Bind the server file descriptor to the server IP address and port number.
    int return_value = set_bind(server, server_info);
    if (return_value == BIND_ERROR_CONTINUE) {
      continue;
    }
    if (return_value == EXIT_FAILURE) {
      (void)stderr_print("Error: SERVER -> get_server_file_descriptor() ");
      return EXIT_FAILURE;
    }
    // The file descriptor was found.
    break;
  }
  // If the server_info is NULL, the bind() failed on all interfaces.
  if (server_info == NULL) {
    (void)stderr_print("Error: SERVER -> bind() : (ptr_server_info == NULL) ");
    (void)stderr_print("Error: SERVER -> get_server_file_descriptor() ");
    return EXIT_FAILURE;
  }
  // Free allocated resource.
  freeaddrinfo(server_info);
  return EXIT_SUCCESS;
}

/**
 * @brief Set the server to listen for incoming connections. If the listen() fails, returns EXIT_FAILURE and the
 * program should end. Otherwise, the function returns EXIT_SUCCESS.
 *
 * @param server file descriptor. The server will be able to accept connections from clients.
 * @return int EXIT_SUCCESS if the listen() was successful, EXIT_FAILURE otherwise.
 */
extern int set_listen(const file_descriptor* server) {
  // The SERVER_BACKLOG is the maximum number of connections that the kernel should queue for this socket. The maximum
  // value for this parameter is SOMAXCONN. The minimum value is 1. The default value in this implementation is 128.
  const int SERVER_BACKLOG = 128;
  // Set the server to listen for incoming connections. The SERVER_BACKLOG parameter defines the maximum length to which
  // the queue of pending connections may grow.
  if (listen(*server, SERVER_BACKLOG) == -1) {
    perror("Error: SERVER -> listen() ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Accept a socket connection and therefore set the file descriptor and IP address of a client.
 *
 * @param server file descriptor.
 * @param client file descriptor.
 * @param client_ip_address string that will store the client IP address.
 * @return int EXIT_SUCCESS if the set_client() was successful, SOCKET_CONTINUE_LISTENING otherwise. If it fails to get
 * the client IP address, returns EXIT_FAILURE.
 */
extern int accept_incoming_connection(file_descriptor* server, file_descriptor* client, char* client_ip_address) {
  int return_value = set_client(server, client, client_ip_address);
  if (return_value == SOCKET_CONTINUE_LISTENING) {
    return SOCKET_CONTINUE_LISTENING;
  }
  if (return_value == EXIT_FAILURE) {
    close_communication(client);
    close_socket(server);
    return EXIT_FAILURE;
  }
  return_value =
      printf("Receive a connection from : %s\n\t ( its file descriptor : %d )\n", client_ip_address, *client);
  if (return_value < 0) {
    perror("Error: SERVER -> accept_incoming_connection() : printf() ");
    close_communication(client);
    close_socket(server);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Receive data from the client.
 *
 * @param server  file descriptor.
 * @param client_ip_address  client IP address.
 * @param client  file descriptor.
 * @param client_data   string that will store the data received from the client.
 * @return int EXIT_SUCCESS if the get_client_data() was successful, SOCKET_CONTINUE_LISTENING otherwise. If the
 * system fails to perform some action, returns EXIT_FAILURE.
 */
extern int receive_client_data(file_descriptor* server, const char* client_ip_address, file_descriptor* client,
                               char* client_data) {
  if (get_client_data(client, client_data) == EXIT_FAILURE) {
    int return_value =
        fprintf(stderr, "Error: SERVER -> unable to receive data from -> %s\n\tClosing communication with client",
                client_ip_address);
    if (return_value < 0) {
      perror("Error: SERVER -> receive_client_data() : fprintf() ");
      close_communication(client);
      close_socket(server);
      return EXIT_FAILURE;
    }
    close_communication(client);
    return SOCKET_CONTINUE_LISTENING;
  }
  if (printf("SERVER: received -> '%s'\n", client_data) < 0) {
    perror("Error: SERVER -> receive_client_data : printf() ");
    close_communication(client);
    close_socket(server);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Send data to the client. If the send() fails, returns EXIT_FAILURE and the program should end.
 * Otherwise, the function returns EXIT_SUCCESS.
 *
 * @param client file descriptor.
 * @param data_to_send string that will be sent to the client.
 * @return int EXIT_SUCCESS if the send() was successful, EXIT_FAILURE otherwise.
 */
extern int send_data_to_client(const file_descriptor* client, const char* data_to_send) {
  // The send() function returns the number of bytes sent. If the number of bytes sent is -1, the send() function
  // failed. If the number of bytes sent is 0, the client has closed the connection. The send() function sends data only
  // to a socket in a connected state.
  ssize_t return_value = send(*client, data_to_send, strlen(data_to_send), 0);
  if (return_value == -1) {
    perror("Error: SERVER -> send() failed ");
    return EXIT_FAILURE;
  }
  if (return_value == 0) {
    perror("Error: SERVER -> send() closed connection ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
/* Send data to Client. */
extern int send_client_data(file_descriptor* server, const char* client_ip_address, file_descriptor* client,
                            const char* data_to_client) {
  if (send_data_to_client(client, data_to_client) == EXIT_FAILURE) {
    int return_value =
        fprintf(stderr, "Error: SERVER -> unable to send data to -> %s\n\tClosing communication with client",
                client_ip_address);
    if (return_value < 0) {
      perror("Error: SERVER -> listening_mode : printf() ");
    }
    /* The communication with this Client is corrupted, we are unable to send data. */
    close_communication(client);
    close_socket(server);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
