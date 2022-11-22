#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "lab_socket/lab_socket.h"

int main(void) {
  struct addrinfo* server_definition = set_server_definition();
  int ret_val = 0;
  if (server_definition == NULL) {
    /* TODO: transform error printing in a function. */
    ret_val = fprintf(stderr, "ERROR: SERVER -> failed to set server_definition.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
