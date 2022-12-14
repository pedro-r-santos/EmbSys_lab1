/**
 * @file main.c
 * @author Pedro R. Santos (pedro.r.santos@estudantes.ips.pt)
 * @brief
 */
#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "lab1_print/embsys_lab1_print.h"
#include "lab1_socket/embsys_lab1_socket.h"

#define BAR "********************************************************************************\n"

static void server_waiting_for_client(void) {
  (void)stdout_print(BAR);
  (void)stdout_print("SERVER  -> listening for connections...\n");
}

typedef struct {
  file_descriptor* server;
  file_descriptor* client;
  char* client_ip_address;
  char* client_data;
} thread_client;

static void* client_communication(void* ptr_thread_client) {
  thread_client* client_data = (thread_client*)ptr_thread_client;
  int* FAIL = malloc(sizeof(int));
  *FAIL = EXIT_FAILURE;
  while (1) {
    // Retrieve the data send by a client.
    int return_value =
        receive_client_data(client_data->client_ip_address, client_data->client, client_data->client_data);
    if (return_value == EXIT_FAILURE) {
      (void)fprintf(
          stderr,
          "Error: SERVER  ->  client_communication() ->  receive_client_data() unable to receive data from '%s'\n",
          client_data->client_ip_address);
      return (void*)FAIL;
    }
    for (unsigned long i = 0; i < strlen(client_data->client_data); i++) {
      if (client_data->client_data[i] == '\0' || client_data->client_data[i] == '\n') {
        puts(" new line \n");
      }
      printf("%c", client_data->client_data[i]);
    }
  }
}

/* The Server is always listening for new connections. */
static int server_listening_loop(file_descriptor* server) {
  while (1) {
    thread_client* client_data = malloc(sizeof(thread_client));
    client_data = &(((thread_client){
        .server = server,
        .client = malloc(sizeof(int)),
        .client_ip_address = malloc(sizeof(char) * INET6_ADDRSTRLEN),  // IPv4 fits in IPv6.
        .client_data = malloc(sizeof(char) * MAX_CLIENT_DATA),
    }));
    // Welcoming text.
    server_waiting_for_client();
    // Accept an incoming connection.
    int return_value = accept_incoming_connection(server, client_data->client, client_data->client_ip_address);
    if (return_value == SOCKET_CONTINUE_LISTENING) {
      continue;
    }
    if (return_value == EXIT_FAILURE) {
      free(client_data->client_ip_address);
      free(client_data->client_data);
      close_socket(client_data->client);
      return EXIT_FAILURE;
    }
    // The lab. specifies that we should only accept one connection. So we wait for the client to close the connection.
    pthread_t listening_thread = 0;
    pthread_create(&listening_thread, NULL, client_communication, (void*)client_data);
    int* thread_return = NULL;
    pthread_join(listening_thread, (void**)&thread_return);
    if (*thread_return == EXIT_FAILURE) {
      close_socket(client_data->client);
      free(client_data->client_ip_address);
      free(client_data->client_data);
      free(client_data->client);
      // free(client_data.server);
      free(thread_return);
      continue;  // We don't want to close the server.
    }
  }  // while (listening)
  return EXIT_SUCCESS;
}

int main(void) {
  // Create a socket.
  file_descriptor* server = malloc(sizeof(int));
  if (get_server_file_descriptor(server) == EXIT_FAILURE) {
    free(server);
    return EXIT_FAILURE;
  }
  // Bind the socket to an address.
  if (set_server_listen(server) == EXIT_FAILURE) {
    free(server);
    return EXIT_FAILURE;
  }
  // Listen for connections. If we got one accept it. If we got data from the client, work with it. If we want to send
  // data to the client, do it.
  if (server_listening_loop(server) == EXIT_FAILURE) {
    close_socket(server);
    free(server);
    return EXIT_FAILURE;
  }
  // Close the socket. Free the resources. Exit.
  close_socket(server);
  free(server);
  return EXIT_SUCCESS;
}
