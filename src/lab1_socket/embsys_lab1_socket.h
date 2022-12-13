
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

/*****************************************************************************/
/*************************** Variables ***************************************/
/*****************************************************************************/

/**
 * @brief The file descriptor type. This is used to represent a socket. Makes the code more readable. It
 * is not necessary to use this type, you can use int instead.
 */
typedef int file_descriptor;

/**
 * @brief The server port to connect to.
 */
extern const char* const SERVER_PORT;

/**
 * @brief The SERVER_BACKLOG is the maximum number of connections that the kernel should queue for this socket. The
 * maximum value for this parameter is SOMAXCONN. The minimum value is 1. The default value in this implementation is
 * 128.
 */
extern const int SERVER_BACKLOG;

/**
 * @brief  This is the maximum number of bytes that can be received from a client.
 */
extern const unsigned int MAX_CLIENT_DATA;

/**
 * @brief The server must keep listening for incoming connections.§
 */
extern const int SOCKET_CONTINUE_LISTENING;

/*****************************************************************************/
/*************************** Functions ***************************************/
/*****************************************************************************/

/**
 * @brief This function will create a server file descriptor. If the function fails, returns EXIT_FAILURE, otherwise
 * returns EXIT_SUCCESS.
 *
 * @param server file descriptor. This is a pointer to a structure of type file_descriptor.
 * @return int EXIT_FAILURE if the function fails, otherwise returns EXIT_SUCCESS.
 */
extern int get_server_file_descriptor(file_descriptor* server);

/**
 * @brief This function will set the server to listen for incoming connections. The SERVER_BACKLOG parameter defines
 + the maximum length to which the queue of pending connections may grow. If the function fails, returns EXIT_FAILURE,
 * otherwise returns EXIT_SUCCESS.
 *
 * @param server file descriptor. This is a pointer to a structure of type file_descriptor.
 * @return int EXIT_FAILURE if the function fails, otherwise returns EXIT_SUCCESS.
 */
extern int set_server_listen(file_descriptor* server);

/**
 * @brief Close the socket connection. And free the file descriptor malloced memory.
 *
 * @param open_socket file descriptor. This is a pointer to a structure of type file_descriptor.
 * @return int EXIT_FAILURE if the function fails, otherwise returns EXIT_SUCCESS.
 */
extern int close_socket(file_descriptor* open_socket);

/**
 * @brief This function will accept an incoming connection. If the function fails to get the client file descriptor
 * returns SOCKET_CONTINUE_LISTENING, if a system error occurs returns EXIT_FAILURE, otherwise returns
 * EXIT_SUCCESS. If the function fails, the client file descriptor will be closed and freed and the client_ip_address
 * will be freed. If the function succeeds, the client file descriptor and IP address will be known.
 *
 * @param server file descriptor. This is a pointer to a structure of type file_descriptor.
 * @param client file descriptor. This is a pointer to a structure of type file_descriptor.
 * @param client_ip_address client IP address. This is a pointer of type char.
 * @return int SOCKET_CONTINUE_LISTENING if the function fails to get the file descriptor, EXIT_FAILURE if a system
 * error occurs, otherwise returns EXIT_SUCCESS.
 */
extern int accept_incoming_connection(file_descriptor* server, file_descriptor* client, char* client_ip_address);

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
                               char* client_data);

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
                               const char* data_to_client);

#endif
