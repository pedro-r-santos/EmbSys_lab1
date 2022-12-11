
#ifndef EMBSYS_LAB1_SOCKET_H_
#define EMBSYS_LAB1_SOCKET_H_

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../lab1_print/embsys_lab1_print.h"

typedef int file_descriptor;

/* Keep listening. */
static const int SOCKET_CONTINUE_LISTENING = -101;

/* Length of the "data string" received. */
static const unsigned int MAX_CLIENT_DATA = 100;

/**
 * @brief Close the communication and delete the file descriptor. If the close() fails, the program is ended.
 *
 * @param open_socket file descriptor. The file descriptor is deleted.
 * @return int EXIT_SUCCESS if the close() was successful, EXIT_FAILURE otherwise.
 */
extern int close_communication(const file_descriptor* open_socket);

/**
 * @brief This function will find a file descriptor for the server. The server will be able to accept connections from
 * clients, send and receive messages and close communications. If the function fails, it returns a EXIT_FAILURE
 * and the program is ended. Otherwise, the function returns EXIT_SUCCESS.
 *
 * @param server file descriptor.
 * @return int EXIT_SUCCESS if the function was successful, EXIT_FAILURE otherwise.
 */
extern int get_server_file_descriptor(file_descriptor* server);

/**
 * @brief Set the server to listen for incoming connections. If the listen() fails, returns EXIT_FAILURE and the
 * program should end. Otherwise, the function returns EXIT_SUCCESS.
 *
 * @param server file descriptor. The server will be able to accept connections from clients.
 * @return int EXIT_SUCCESS if the listen() was successful, EXIT_FAILURE otherwise.
 */
extern int set_listen(const file_descriptor* server);

/**
 * @brief Close the socket connection. And free the file descriptor malloced memory.
 *
 * @param server file descriptor.
 */
extern void close_socket(file_descriptor* open_socket);

/**
 * @brief Accept a socket connection and therefore set the file descriptor and IP address of a client.
 *
 * @param server file descriptor.
 * @param client file descriptor.
 * @param client_ip_address string that will store the client IP address.
 * @return int EXIT_SUCCESS if the set_client() was successful, SOCKET_CONTINUE_LISTENING otherwise. If it fails to get
 * the client IP address, returns EXIT_FAILURE.
 */
extern int accept_incoming_connection(file_descriptor* server, file_descriptor* client, char* client_ip_address);

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
                               char* client_data);

extern int send_client_data(file_descriptor* server, const char* client_ip_address, file_descriptor* client,
                            const char* data_to_client);

#endif
