/**
 * @brief This program has the objective to serve as a test Client for the
 * Server. It wont have function verifications like the other programs of this
 * lab or any other fail proof / fail safe aspects.
 */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[argc + 1]) {
  if (argc != 4) {
    fprintf(stderr,
            "Error -> CLIENT: usage -> \n"
            "[option 1]./test_client {SERVER_IP} "
            "{CLIENT_SOURCE_NAME [A...z/a...z]} {TIME_IN_mS}\n");
    return EXIT_FAILURE;
  }

  /* Client definition -> IP type, Transport Layer. */
  struct addrinfo client_definition;
  memset(&client_definition, 0, sizeof client_definition);
  client_definition.ai_family = AF_INET6;      /* Use IPv6. */
  client_definition.ai_socktype = SOCK_STREAM; /* Use TCP. */

  /* Server were we will connect. */
  const char* SERVER_PORT = "65432";
  struct addrinfo* server_info = NULL;
  int rv = getaddrinfo(argv[1], SERVER_PORT, &client_definition, &server_info);
  if (rv != 0) {
    fprintf(stderr, "Error -> CLIENT: getaddrinfo(): %s\n", gai_strerror(rv));
    return EXIT_FAILURE;
  }

  /* Loop through all the results and bind to the first we can. */
  struct addrinfo* ptr_server_info = NULL;
  int server_fd = 0;
  for (ptr_server_info = server_info; ptr_server_info != NULL; ptr_server_info = ptr_server_info->ai_next) {
    /* Allocate Server File Descriptor. */
    server_fd = socket(ptr_server_info->ai_family, ptr_server_info->ai_socktype, ptr_server_info->ai_protocol);
    if (server_fd == -1) {
      perror("Error -> CLIENT: socket(): ");
      continue;
    }

    /* Connect Client to Server. */
    rv = connect(server_fd, ptr_server_info->ai_addr, ptr_server_info->ai_addrlen);
    if (rv == -1) {
      close(server_fd);
      perror("Error -> CLIENT: connect(): ");
      continue;
    }
    break;
  }

  if (ptr_server_info == NULL) {
    fprintf(stderr, "Error -> CLIENT: connect(): (ptr_server_info == NULL)\n");
    return EXIT_FAILURE;
  }

  /* free resources. */
  freeaddrinfo(server_info);

  /* send data to Server. */
  const char* data_to_server = "Hello there!";
  ssize_t rv_send = send(server_fd, data_to_server, strlen(data_to_server), 0);
  if (rv_send == -1) {
    perror("send");
    return EXIT_FAILURE;
  }

  /* Get data from Server. */
  const unsigned int MAX_DATA = 100;
  char server_data[MAX_DATA];
  ssize_t number_bytes = recv(server_fd, server_data, MAX_DATA - 1, 0);
  if (number_bytes == -1) {
    perror("recv");
    return EXIT_FAILURE;
  }
  server_data[number_bytes] = '\0';
  printf("CLIENT: received '%s'\n", server_data);
  close(server_fd);

  return EXIT_SUCCESS;
}
