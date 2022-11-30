#ifndef EMBSYS_LAB1_ERROR_HANDLER_H_
#define EMBSYS_LAB1_ERROR_HANDLER_H_

#include <stdio.h>

#define MALLOC_FAILED 2

/* System. */
extern void error_exit_failure(void);
extern void error_malloc_failure(char* place_of_failure);

/* Server. */
extern void error_set_server_info(void);

#endif
