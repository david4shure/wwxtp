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

#include "xml.c"
#include "test_database.c"

#define PORT 4114
#define MAX_BUFFER 512

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

sockaddr_in any_addr () {
    sockaddr_in sa;
    sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(PORT);
    return sa;
}

char* recv_string (int sock) {
    char* buf = GC_malloc(MAX_BUFFER);
    uint msg_size = 0;
    uint got = 0;
    printf("recv...\n");
    got = CHECK(recv, sock, buf, MAX_BUFFER, 0);
        printf("%d\n", got);
        msg_size += got;
        buf = GC_realloc(buf, got + MAX_BUFFER);
    printf("Got a message.\n");
    buf[msg_size] = 0;
    return buf;
}

const char* query_api (const char* lat, const char* lon) {

  struct sockaddr_in sa;
  char buffer[MAX_BUFFER];
  static char RequestBegin[] = "GET /api/e5df4405be67dec2/geolookup/q/";
  static char RequestEnd[] = ".xml\r\n\r\n";
  char Request[strlen(lat) + strlen(lon) + strlen(RequestBegin) + strlen(RequestEnd) + 2];
  Request[0] = 0;
  strcat(Request, RequestBegin);
  strcat(Request, lat);
  strcat(Request, ",");
  strcat(Request, lon);
  strcat(Request, RequestEnd);
  puts(Request);
  static char WunderServer[] = "38.102.136.138";
  int sockfd = CHECK(socket, AF_INET, SOCK_STREAM, 0);
  inet_pton(AF_INET, WunderServer, &(sa.sin_addr));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(80);
  int sockconn = CHECK(connect, sockfd, (struct sockaddr *)&sa, sizeof(sa));
  int socksend = CHECK(send, sockfd, Request, strlen(Request), 0);
  int sockrecv = 1;
  int len = 1;
  char* str = GC_malloc(len);
  str[0] = '\0';
  do {
    sockrecv = CHECK(recv, sockfd, &buffer, MAX_BUFFER, 0);
    if(sockrecv > 0) {
      buffer[sockrecv] = '\0';
      printf("%s", buffer);
      len += sockrecv;
      str = GC_realloc(str, len);
      strcat(str, buffer);
    }
  } while(sockrecv > 0);
  int sockclose = CHECK(close, sockfd);
  puts(str);
  return str;
}

const char* error_response = "<wwxtp>Error!</wwxtp>";

const char* process_message (const char* message) {
    puts("Start processing message...");
    XML wwxtp = XML_parse(message);
    XML request = XML_get_child(wwxtp, "request");
    if (!XML_is_valid(request)) return error_response;
    XML command = XML_get_child(request, "command");
    if (!XML_is_valid(command)) return error_response;
    XML command_text = command.tag->contents[0];
    if (!XML_is_str(command_text)) return error_response;
    if (0!=strcmp(command_text.str, "RETRIEVE")) return error_response;
    XML position = XML_get_child(request, "position");
    if (!XML_is_valid(position)) return error_response;
    const char* lat = XML_get_attr(position, "lat");
    if (!lat) return error_response;
    const char* lon = XML_get_attr(position, "lon");
    if (!lon) return error_response;
    return query_api(lat, lon);
}


int main () {
    GC_init();
    int sock = CHECK(socket, AF_INET, SOCK_STREAM, 0);
    sockaddr_in listen_addr = any_addr();
    printf("bind...\n");
    CHECK(bind, sock, (sockaddr*)&listen_addr, sizeof(listen_addr));
    printf("listen...\n");
    CHECK(listen, sock, SOMAXCONN);
    sockaddr_in client_addr;
    socklen_t client_addr_len;
    int new_sock;
    printf("accept...\n");
    do {
        new_sock = CHECK(accept, sock, (sockaddr*)&client_addr, &client_addr_len);
        char* message = recv_string(new_sock);
        puts(message);
        const char* response = process_message((const char*)message);
        puts("Response generated.");
        CHECK(send, new_sock, response, strlen(response) + 1, 0);
        puts("Response sent.\n");
//        char endbuf [MAX_BUFFER];
//        CHECK(recv, new_sock, endbuf, MAX_BUFFER, 0);
//        puts("Close request recieved.");
        CHECK(close, new_sock);
    } while (0);
    CHECK(close, sock);
    return 0;
}
