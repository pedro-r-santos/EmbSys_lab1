#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "lab1_error_handler/embsys_lab1_error_handler.h"
#include "lab1_socket/embsys_lab1_socket.h"

int main(void) {
  struct addrinfo server_info;
  if (set_server_info(&server_info) != MALLOC_FAILED) {
    error_malloc_failure("set_server_info()");
    error_set_server_info();
    error_exit_failure();
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
