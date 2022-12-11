/**
 * @file main.c
 * @author Pedro R. Santos (pedro.r.santos@estudantes.ips.pt)
 * @brief
 */
#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "lab1_print/embsys_lab1_print.h"
#include "lab1_socket/embsys_lab1_socket.h"

/* Close and free the socket and other resources associated with *server. */
inline void close_server(file_descriptor* server) {
  /* Close the Server socket. */
  close_communication(server);
  /* Free the file descriptor malloced memory. */
  free(server);
}

/* Listen for a connection, and if we got one accept it. */
static int accept_incoming_connection(file_descriptor* server, file_descriptor* client, char* client_ip_address) {
  int return_value = set_client(server, client, client_ip_address);
  if (return_value == SOCKET_CONTINUE_LISTENING) {
    return SOCKET_CONTINUE_LISTENING;
  }
  if (return_value == EXIT_FAILURE) {
    /* Something failed close and free resources. */
    close_communication(client);
    close_server(server);
    return EXIT_FAILURE;
  }
  return_value =
      printf("Receive a connection from : %s\n\t ( its file descriptor : %d )\n", client_ip_address, *client);
  if (return_value < 0) {
    perror("Error: SERVER -> accept_incoming_connection() : printf() ");
    close_communication(client);
    close_server(server);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/* Retrieve the data send by a Client. */
static int receive_client_data(file_descriptor* server, const char* client_ip_address, file_descriptor* client,
                               char* client_data) {
  if (get_client_data(client, client_data) == EXIT_FAILURE) {
    int return_value =
        fprintf(stderr, "Error: SERVER -> unable to receive data from -> %s\n\tClosing communication with client",
                client_ip_address);
    if (return_value < 0) {
      perror("Error: SERVER -> receive_client_data() : fprintf() ");
      close_communication(client);
      close_server(server);
      return EXIT_FAILURE;
    }
    /* The communication with this Client is corrupted. */
    close_communication(client);
    return SOCKET_CONTINUE_LISTENING;
  }
  if (printf("SERVER: received -> '%s'\n", client_data) < 0) {
    perror("Error: SERVER -> receive_client_data : printf() ");
    close_communication(client);
    close_server(server);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/* Send data to Client. */
static int send_client_data(file_descriptor* server, const char* client_ip_address, file_descriptor* client,
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
    close_server(server);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/* The Server is always listening for new connections. */
static int server_listening_loop(file_descriptor* server) {
  file_descriptor* client = malloc(sizeof(int));
  char* client_ip_addr = malloc(sizeof(char) * INET6_ADDRSTRLEN);
  char* client_data = malloc(sizeof(char) * MAX_CLIENT_DATA);
  int return_value = 0;

  while (true) {
    if (printf("SERVER -> waiting for connections...\n") < 0) {
      perror(
          "Error: SERVER -> listening_mode : "
          "printf() \n");
      close_server(server);
      return EXIT_FAILURE;
    }

    /* Accept an incoming connection. */
    return_value = accept_incoming_connection(server, client, client_ip_addr);

    if (return_value == CONTINUE) {
      continue;
    } else if (return_value == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }

    /* Get data from Client. */
    return_value = receive_client_data(server, client_ip_addr, client, client_data);

    if (return_value == EXIT_FAILURE) {
      return EXIT_FAILURE;
    } else if (return_value == CONTINUE) {
      continue;
    }

    /* Send data to Client. */
    const char* data_to_client = "General Kenobi!";
    return_value = send_client_data(server, client_ip_addr, client, data_to_client);
    if (return_value == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
    close_communication(client);
  }
}

int main(void) {
  /* Server definition -> IP type, Transport Layer and IP address. */
  struct addrinfo server_protocols;
  if (set_server_protocol(&server_protocols) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  struct addrinfo* server_info = NULL; /* Memory freed inside sert. */
  int return_value = set_server_addr_info(&server_protocols, &server_info);
  if (return_value != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }
  int* server_file_descriptor = malloc(sizeof(int));
  if (get_server_file_descriptor(server_file_descriptor, server_info) == EXIT_FAILURE) {
    /* server socket is closed inside get_server_file_descriptor() */
    free(server_file_descriptor);
    return EXIT_FAILURE;
  }
  if (set_listen(server_file_descriptor) == EXIT_FAILURE) {
    close_server(server_file_descriptor);
    return EXIT_FAILURE;
  }
  /* Server listens for new connections. */
  if (server_listening_loop(server_file_descriptor) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
