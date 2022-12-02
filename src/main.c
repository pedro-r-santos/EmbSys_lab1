#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "lab1_socket/embsys_lab1_socket.h"

static void close_server(int* server_file_descriptor) {
  close_communication(server_file_descriptor);
  free(server_file_descriptor);
}

int main(void) {
  /* Server definition -> IP type, Transport Layer and IP address. */
  struct addrinfo server_protocols;
  if (set_server_protocol(&server_protocols) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  struct addrinfo* server_info = NULL; /* Memory freed inside get_server_file_descriptor. */
  int return_value = set_server_addr_info(&server_protocols, &server_info);
  if (return_value != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }
  int* server_file_descriptor = malloc(sizeof(int));
  if (get_server_file_descriptor(server_file_descriptor, server_info) == EXIT_FAILURE) {
    free(server_file_descriptor);
    return EXIT_FAILURE;
  }
  // if (set_listen(server_file_descriptor) == EXIT_FAILURE) {
  if (set_listen(server_file_descriptor) == EXIT_FAILURE) {
    close_server(server_file_descriptor);
    return EXIT_FAILURE;
  }

  bool listening_mode = true;
  while (listening_mode) {
    if (printf("SERVER -> waiting for connections...\n") < 0) {
      perror("Error: SERVER -> listening_mode : printf() : \n");
      close_server(server_file_descriptor);
      return EXIT_FAILURE;
    }
    /* Accept an incoming connection. */
    int* client_file_descriptor = malloc(sizeof(int));
    char* client_ip_addr = malloc(sizeof(char) * INET6_ADDRSTRLEN);
    return_value = set_client_file_descriptor(server_file_descriptor, client_file_descriptor, client_ip_addr);
    if (return_value == CONTINUE) {
      continue;
    }
    if (return_value == EXIT_FAILURE) {
      close_server(server_file_descriptor);
      return EXIT_FAILURE;
    }
    printf("Got connection from : %s\n\t with and fd : %d\n", client_ip_addr, *client_file_descriptor);
    /* Get data from Client. */
    char* client_data = malloc(sizeof(char) * MAX_CLIENT_DATA);
    if (get_client_data(client_file_descriptor, client_data) == EXIT_FAILURE) {
      fprintf(stderr, "Error: SERVER -> unable to receive data from -> %s\n", client_ip_addr);
      continue;
    }
    printf("SERVER: received -> '%s'\n", client_data);
  }
  free(server_file_descriptor);
  return EXIT_SUCCESS;
}
