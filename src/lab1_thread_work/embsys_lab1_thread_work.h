#ifndef EMBSYS_LAB1_THREAD_WORK_H_
#define EMBSYS_LAB1_THREAD_WORK_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lab1_print/embsys_lab1_print.h"
#include "../lab1_queue/embsys_lab1_queue.h"
#include "../lab1_socket/embsys_lab1_socket.h"

typedef struct {
  file_descriptor* server;
  file_descriptor* client;
  char* client_ip_address;
  char* client_data;
} thread_client;

extern void* client_communication(void* ptr_thread_client);
extern void* handle_client_raw_data(void* ptr_queue_raw_data);

#endif  // EMBSYS_LAB1_THREAD_WORK_H_
