#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#define PORT 4114
#define MAX_BUFFER 512
#define LOCAL_HOST "127.0.0.1"

int main () {

  int our_socket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in our_address;
  our_address.sin_family = AF_INET;
  inet_pton(AF_INET, LOCAL_HOST, &our_address.sin_addr);
  our_address.sin_port = htons(PORT);

  // Binding our socket with a port and an IP, in our case localhost (127.0.0.1)
  if (bind(our_socket, (struct sockaddr *) &our_address, sizeof(our_address)) == -1) {
    printf("Failed to bind with errno %d\n", errno);
    return errno;
  }

  // Dealing with errors while attempted to listen
  if (listen(our_socket, SOMAXCONN) == -1) {
    printf("Failed to listen. relevant info : %d\n", errno);
    return errno;
  }

  // Variable that holds our incoming data
  char* buffer = GC_malloc(MAX_BUFFER);
  int recv_size = 0;
  int msg_size = 0;
  while (recv_size = recv(our_socket, buffer + msg_size, MAX_BUFFER, 0)) {
    
    if (recv_size == -1) {
      printf("recv returned with errno %d\n", errno);
      return errno;
    }
    msg_size += recv_size;
    buffer = GC_realloc(buffer, msg_size + MAX_BUFFER);

  }
  buffer[msg_size] = 0;
  puts(buffer);
}
