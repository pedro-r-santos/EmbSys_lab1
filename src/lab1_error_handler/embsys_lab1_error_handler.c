#include "embsys_lab1_error_handler.h"

/* System. */
extern void error_exit_failure() {
  fprintf(stderr, "\tEXIT FAILURE.\n");
}
extern void error_malloc_failure(char* place_of_failure) {
  fprintf(stderr, "ERROR: SYSTEM -> malloc failed in %s.\n", place_of_failure);
}

/* Server. */
extern void error_set_server_info() {
  fprintf(stderr, "ERROR: SERVER -> unable to set server info.\n");
}
