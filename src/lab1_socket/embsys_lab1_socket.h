
#ifndef EMBSYS_LAB1_SOCKET_H_
#define EMBSYS_LAB1_SOCKET_H_

#include <../lab1_print/embys_lab1_print.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int file_descriptor;

extern const int SOCKET_CONTINUE = -101;

/* Length of the "data string" received. */
extern const unsigned int MAX_CLIENT_DATA = 100;

/* Define the Internet Protocols that the Server will use. */
extern int set_server_protocol(struct addrinfo *protocols);

/* Set the Server address structure and service name. */
extern int set_server_addr_info(struct addrinfo *protocols,
				struct addrinfo **server_info);

/* Close communications and delete the file descriptor. */
extern int close_communication(const file_descriptor *fd);

/* Find a file descriptor for Server. */
extern int get_server_file_descriptor(file_descriptor *server,
				      struct addrinfo *server_info);

/* Listen for connections on Server socket. */
extern int set_listen(const file_descriptor *server);

/*
 * Accept a socket connection and therefore set the file descriptor and IP
 * address of a Client.
 */
extern int set_client(const file_decriptor *server, file_descriptor *client,
		      char *client_ip_addr);

/* Get data from the Client. */
extern int get_client_data(const file_descriptor *client, char *client_data);

/* Send data to a Client. */
extern int send_data_to_client(const file_descriptor *client,
			       const char *data_to_send);

#endif
