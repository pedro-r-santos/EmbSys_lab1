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
  /*
   * Make a thread that as the duty of pthread_cancel other threads when an error occurs.
   * After canceling the threads it must write in each client file the warning message of the system interruption.
   * The fist thread  to cancel is the one that receives socket communications. */
  struct addrinfo server_info;
  if (set_server_protocols(&server_info) == MEMSET_FAILED) {
    error_memset_failure("set_server_info()");
    error_set_server_protocols();
    error_exit_failure();
    return EXIT_FAILURE;
  }

  int* server_filed_descriptor = set_server_file_descriptor(&server_info);
  if (server_filed_descriptor == NULL) {
    free(server_filed_descriptor);
    error_set_server_filed_descriptor();
    error_exit_failure();
    return EXIT_FAILURE;
  }

  free(server_filed_descriptor);
  return EXIT_SUCCESS;
}
