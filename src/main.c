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

/* The Server is always listening for new connections. */
static int server_listening_loop(file_descriptor* server) {
  int return_value = 0;
  bool listening = true;
  while (listening) {
    file_descriptor* client = malloc(sizeof(int));
    char* client_ip_addr = malloc(sizeof(char) * INET6_ADDRSTRLEN);  // IPv4 fits in IPv6.
    char* client_data = malloc(sizeof(char) * MAX_CLIENT_DATA);
    // Welcoming text.
    server_waiting_for_client();
    // Accept an incoming connection.
    return_value = accept_incoming_connection(server, client, client_ip_addr);
    if (return_value == SOCKET_CONTINUE_LISTENING || return_value == EXIT_FAILURE) {
      // client is closed and freed in receive_client_data().
      listening = !(return_value == EXIT_FAILURE);
      goto STOP_LISTENING;
    }
    // Retrieve the data send by a client.
    return_value = receive_client_data(server, client_ip_addr, client, client_data);
    if (return_value == SOCKET_CONTINUE_LISTENING || return_value == EXIT_FAILURE) {
      // client is closed and freed in receive_client_data().
      listening = !(return_value == EXIT_FAILURE);
      goto STOP_LISTENING;
    }
    // Print received data
    if (printf("SERVER  -> received data from client -> %s : '%s'\n", client_ip_addr, client_data) < 0) {
      close_socket(client);
      close_socket(server);
      listening = false;
      goto STOP_LISTENING;
    }
    // Send data to the Client.
    const char* data_to_client = "General Kenobi!";
    return_value = send_data_to_client(server, client_ip_addr, client, data_to_client);
    if (return_value == EXIT_FAILURE) {
      // client is closed and freed in send_data_to_client().
      listening = false;
      goto STOP_LISTENING;
    }
    close_socket(client);  // malloced memory is freed in close_socket().
  STOP_LISTENING:
    if (listening == false) {
      free(client_ip_addr);
      free(client_data);
      return EXIT_FAILURE;
    }
    free(client_ip_addr);
    free(client_data);

// TODO: TEST -> Remove this.
#define EXIT_LOOP_AFTER_5_CONNECTIONS 5
    static int i = 0;
    if (++i == EXIT_LOOP_AFTER_5_CONNECTIONS) {
      listening = false;
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
    close_socket(server);
    return EXIT_FAILURE;
  }
  // Listen for connections. If we got one accept it. If we got data from the client, work with it. If we want to send
  // data to the client, do it.
  if (server_listening_loop(server) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  // Close the socket. Free the resources. Exit.
  close_socket(server);
  return EXIT_SUCCESS;
}
