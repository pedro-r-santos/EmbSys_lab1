#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "lab_socket/lab_socket.h"

int main(void) {
  struct addrinfo server_info;
  if (set_server_info(&server_info) == EXIT_FAILURE) {
    /* TODO: implemente error handler. */
  }
  return EXIT_SUCCESS;
}
