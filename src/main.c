#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "lab1_socket/embsys_lab1_socket.h"

int main(void) {
  struct addrinfo server_info;
  if (set_server_info(&server_info) == EXIT_FAILURE) {
    /* TODO: implemente error handler. */
  }
  return EXIT_SUCCESS;
}
