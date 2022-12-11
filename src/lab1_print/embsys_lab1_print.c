#include "embsys_lab1_print.h"

extern int stderr_print(char* str) {
  if (fprintf(stderr, "%s", str) < 0) {
    strcat(str, ": fprintf() ");
    perror(str);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

extern int stdout_print(char* str) {
  if (printf("%s", str) < 0) {
    strcat(str, ": printf() ");
    perror(str);
    return EXIT_FAILURE;
  }
  return EXIT_FAILURE;
}
