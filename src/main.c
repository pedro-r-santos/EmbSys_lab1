#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "lab1_error_message/embsys_lab1_error_message.h"
#include "lab1_socket/embsys_lab1_socket.h"

int main(void) {
  int rv = 0;
  /* Server definition -> IP type, Transport Layer and IP address. */
  struct addrinfo server_protocols;
  if (set_server_protocol(&server_protocols) == MEMSET_ERROR) {
    /* gai_strerror(), get error message string from EAI_xxx error code. */
    fprintf(stderr, "Error: SERVER -> set_server_protocol() : memset() failed.\n");
    return EXIT_FAILURE;
  }
  struct addrinfo* server_info = NULL; /* Memory freed inside get_server_file_descriptor. */
  int return_value = set_server_addr_info(&server_protocols, &server_info);
  if (return_value != EXIT_SUCCESS) {
    fprintf(stderr, "Error: SERVER -> getaddrinfo(): %s\n", gai_strerror(return_value));
    return EXIT_FAILURE;
  }
  int* server_file_descriptor = (int*)malloc(sizeof(int));
  if (get_server_file_descriptor(server_file_descriptor, server_info) == EXIT_FAILURE) {
    fprintf(stderr, "Error: SERVER -> get_server_file_descriptor().\n");
    free(server_file_descriptor);
    return EXIT_FAILURE;
  }
  if (set_listen(server_file_descriptor) == EXIT_FAILURE) {
    fprintf(stderr, "Error: SERVER -> set_listen().\n");
    free(server_file_descriptor);
    return EXIT_FAILURE;
  }
  free(server_file_descriptor);
  return EXIT_SUCCESS;
}
