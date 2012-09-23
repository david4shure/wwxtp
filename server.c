#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <gc/gc.h>
#include <stdlib.h>


#define PORT 4114
#define MAX_BUFFER 512
#define LOCAL_HOST "127.0.0.1"

#define CHECK(f, ...) ({ int ret = f(__VA_ARGS__); if (ret == -1) {fprintf(stderr, "%s() returned error: %d (%s)\n", #f, errno, strerror(errno)); exit(errno); }; ret; })

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

sockaddr_in ip_addr (const char* addr) {
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    inet_pton(AF_INET, addr, &sa.sin_addr);
    sa.sin_port = htons(PORT);
    return sa;
}

char* recv_string (int sock) {
    char* buf = GC_malloc(MAX_BUFFER);
    uint msg_size = 0;
    uint got = 0;
    while (got = CHECK(recv, sock, buf, MAX_BUFFER, 0)) {
        msg_size += got;
        buf = GC_realloc(buf, got + MAX_BUFFER);
    }
    buf[msg_size] = 0;
    return buf;
}

int main () {
    int sock = CHECK(socket, AF_INET, SOCK_STREAM, 0);
    sockaddr_in listen_addr = ip_addr("127.0.0.1");
    CHECK(bind, sock, (sockaddr*)&listen_addr, sizeof(listen_addr));
    CHECK(listen, sock, SOMAXCONN);
    sockaddr_in client_addr;
    socklen_t client_addr_len;
    int new_sock;
    while (new_sock = CHECK(accept, sock, (sockaddr*)&client_addr, &client_addr_len)) {
        char* message = recv_string(new_sock);
        puts(message);
        CHECK(send, new_sock, "Hi there, I got your message!\n", 31, 0);
        char endbuf [MAX_BUFFER];
        CHECK(recv, new_sock, endbuf, MAX_BUFFER, 0);
        CHECK(close, new_sock);
    }
    return 0;
}
