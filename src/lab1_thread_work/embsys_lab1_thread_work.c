#include "embsys_lab1_thread_work.h"

#include <stdlib.h>
#include <unistd.h>

// const int* const FAILED_THREAD = (int*)EXIT_FAILURE;
// const int* const SUCCESSFUL_THREAD = (int*)EXIT_SUCCESS;

static inline int* failed_thread() {
  int* FAILED_THREAD = malloc(sizeof(int));
  *FAILED_THREAD = EXIT_FAILURE;
  return FAILED_THREAD;
}

static inline int* successful_thread() {
  int* SUCCESSFUL_THREAD = malloc(sizeof(int));
  *SUCCESSFUL_THREAD = EXIT_SUCCESS;
  return SUCCESSFUL_THREAD;
}

extern void* client_communication(void* ptr_thread_client) {
  thread_client* clint_info = (thread_client*)ptr_thread_client;
  int* FAILED_THREAD = failed_thread();
  int* SUCCESS_THREAD = successful_thread();
  // create a queue to save raw data
  queue_t* raw_data_queue = NULL;
  queue_create(raw_data_queue);
  // thread that will parse raw data
  pthread_t thread_handle_raw_data = 0;
  pthread_create(&thread_handle_raw_data, NULL, handle_client_raw_data, (void*)raw_data_queue);
  while (1) {
    // Retrieve the data send by a client.
    int return_value = receive_client_data(clint_info->client_ip_address, clint_info->client, clint_info->client_data);
    if (return_value == EXIT_FAILURE) {
      (void)fprintf(
          stderr,
          "Error: SERVER  ->  client_communication() ->  receive_client_data() unable to receive data from '%s'\n",
          clint_info->client_ip_address);
      pthread_exit((void*)FAILED_THREAD);
    }
    // save raw data to queue
    for (unsigned long i = 0; i < strlen(clint_info->client_data); i++) {
      queue_enqueue(raw_data_queue, (void*)&clint_info->client_data[i]);
    }
  }
  pthread_exit(SUCCESS_THREAD);
}

extern void* handle_client_raw_data(void* ptr_queue_raw_data) {
  queue_t* raw_data_queue = (queue_t*)ptr_queue_raw_data;
  int* FAILED_THREAD = failed_thread();
  int* SUCCESS_THREAD = successful_thread();
  while (1) {
    // get raw data from queue
    char* raw_data = NULL;
    sleep(1);
    queue_dequeue(raw_data_queue, (void**)&raw_data);
    if (*raw_data == '\0' || *raw_data == '\n') {
      puts("\n");
    } else {
      printf("%c", *raw_data);
    }
  }
  pthread_exit(SUCCESS_THREAD);
}
