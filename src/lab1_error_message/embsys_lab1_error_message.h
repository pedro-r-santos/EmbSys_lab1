#ifndef EMBSYS_LAB1_ERROR_HANDLER_H_
#define EMBSYS_LAB1_ERROR_HANDLER_H_

#include <errno.h>
#include <netdb.h>
#include <stdio.h>

/* System. */
#define MEMSET_FAILED 2
extern void error_exit_failure(void);
extern void error_memset_failure(char* place_of_failure);

/* Server. */
extern void error_set_server_protocols(void);
extern void error_get_addr_info(int error);
extern void error_set_socket(void);
extern void error_set_sock_opt(void);
extern void error_set_bind(void);
extern void error_close(void);
extern void error_addr_info(void);
extern void error_set_server_filed_descriptor(void);

#endif
