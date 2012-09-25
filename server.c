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

const char* error_response = "<wwxtp>Error!</wwxtp>";

const char* process_message (const char* message) {
    puts("Start processing message...");
    XML wwxtp = XML_parse(message);
    XML query = XML_get_child(wwxtp, "query");
    XML command = XML_get_child(query, "command");
    XML command_text = command.tag->contents[0];
    if (!XML_is_str(command_text)) return error_response;
    if (0!=strcmp(command_text.str, "TEST")) return error_response;
    XML id = XML_get_child(query, "id");
    XML id_text = id.tag->contents[0];
    if (!XML_is_str(id_text)) return error_response;
    printf("ID: %s\n", id_text.str);
     // search database
    uint i;
    for (i = 0; i < TEST_DATABASE_SIZE; i++) {
        if (0==strcmp(test_database[i].id, id_text.str)) {
            puts("Building xml...");
            return XML_as_text(
                XML_tag( "wwxtp", NULL,
                    XML_tag( "response", NULL,
                        XML_tag( "id", NULL,
                            test_database[i].id,
                            NULL
                        ),
                        XML_tag( "city", NULL,
                            test_database[i].city,
                            NULL
                        ),
                        XML_tag( "name", NULL,
                            test_database[i].name,
                            NULL
                        ),
                        XML_tag( "where", NULL,
                            test_database[i].where,
                            NULL
                        ),
                        NULL
                    ),
                    NULL
                )
            );
        }
    }
    return "<wwxtp>Error: Not found</wwxtp>";
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
