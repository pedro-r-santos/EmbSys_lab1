#include "embsys_lab1_error_message.h"

/* System. */
extern void error_exit_failure(void) {
  fprintf(stderr, "\tEXIT FAILURE.\n");
}

extern void error_memset_failure(char* place_of_failure) {
  fprintf(stderr, "ERROR: SYSTEM -> memset(): failed in %s.\n", place_of_failure);
}

/* Server. */
extern void error_set_server_protocols(void) {
  fprintf(stderr, "ERROR: SERVER -> unable to set server protocols.\n");
}
extern void error_get_addr_info(int error) {
  fprintf(stderr, "Error: SERVER -> getaddrinfo(): %s.\n", gai_strerror(error));
}
extern void error_set_socket(void) {
  fprintf(stderr, "Error: SERVER -> socket(): ");
}

extern void error_set_sock_opt(void) {
  fprintf(stderr, "Error: SERVER -> setsockopt(): ");
}

extern void error_set_bind(void) {
  fprintf(stderr, "Error: SERVER -> bind(): ");
}
extern void error_close(void) {
  fprintf(stderr, "Error: SERVER -> close(): ");
}
extern void error_addr_info(void) {
  fprintf(stderr, "Error: SERVER -> bind(): (ptr_server_info == NULL).\n");
}
extern void error_set_server_filed_descriptor(void) {
  fprintf(stderr, "Error: SERVER -> failed to obtain a file descriptor to identify server.\n");
}
