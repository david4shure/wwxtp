#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <gc/gc.h>

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
    printf("Failed to bind with errno %d (%s)\n", errno, strerror(errno));
    return errno;
  }

  // Dealing with errors while attempted to listen
  if (listen(our_socket, SOMAXCONN) == -1) {
    printf("Failed to listen. relevant info : %d (%s)\n", errno, strerror(errno));
    return errno;
  }

  // Variable that holds our incoming data
  char* buffer = GC_MALLOC(MAX_BUFFER);
  int recv_size = 0;
  int msg_size = 0;
//  int conn;
//  while (conn = accept(our_socket, NULL, NULL) != -1) {
//    int select_r;
//    fd_set fds;
//    do {
//      FD_ZERO(&fds);
//      FD_SET(conn, &fds);
//      struct timeval tv;
//      tv.tv_sec = 30;
//      tv.tv_usec = 0;
//      select_r = select(conn + 1, &fds, NULL, NULL, &tv);
//    } while (select_r == -1 && errno == EINTR);
//    if (select_r > 0) {
//      if (FD_ISSET(conn, &fds)) {
        while (recv_size = recvfrom(our_socket, buffer + msg_size, MAX_BUFFER, 0, NULL, NULL)) {    
          if (recv_size == -1) {
            printf("recv returned with errno %d (%s)\n", errno, strerror(errno));
            return errno;
          }
          msg_size += recv_size;
          buffer = GC_REALLOC(buffer, msg_size + MAX_BUFFER);
          buffer[msg_size] = 0;
          puts(buffer);
        }
        buffer[msg_size] = 0;
        puts(buffer);
//      }
//    }
//    else {
//      printf("select() barfed: %d (%s)\n", errno, strerror(errno));
//      return errno;
//    }
//  }
  printf("Accept errored! %d (%s)\n", errno, strerror(errno));
  return errno;

  return 0;
}
