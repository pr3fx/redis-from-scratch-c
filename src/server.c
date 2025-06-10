#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>


#include "server.h"

#define CONN_BACKLOG_N 5
#define PING_REQ  "*1\r\n$4\r\nPING\r\n\0"
#define PING_RESP "+PONG\r\n"

void server_start()
{
    int server_fd;

    // ------------------------- CREATE SOCKET ------------------------- //
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Could not create socket object: %s\n", strerror(errno));
        exit(1);
    }

	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
        exit(1);
	}

    // ------------------------ BIND TO ADDRESS ------------------------ //
    struct sockaddr_in server_addr = { .sin_family = AF_INET,
                                       .sin_port = htons(6379),
                                       .sin_addr = { htonl(INADDR_ANY) },
    };

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        printf("Could not bind: %s\n", strerror(errno));
        exit(1);
    }

    // ---------------------------- LISTEN ----------------------------- //
    if (listen(server_fd, CONN_BACKLOG_N) != 0) {
        printf("Could not listen: %s\n", strerror(errno));
        exit(1);
    }

    // Start accept a client
    printf("Awaiting client connection...\n");
    accept_conn(server_fd);

    close(server_fd);
}


void accept_conn(int server_fd)
{
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    
    // Accept a client
    int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
    if (client_fd == -1) {
        printf("Accepting client failed: %s\n", strerror(errno));
        return;
    }

    char recv_buf[128];
    size_t recv_buf_len = sizeof(char)*sizeof(recv_buf);
    memset(recv_buf, 0, recv_buf_len);

    // Listen for requests
    while (1) {
        if (recv(client_fd, recv_buf, recv_buf_len, 0) == -1) {
            printf("Could not receive bytes from client: %s\n", strerror(errno));
            continue;
        }

        // Check for request type and respond
        if (!strcmp(PING_REQ, recv_buf)) {
            if (send(client_fd, PING_RESP, strlen(PING_RESP), 0) == -1) {
                printf("Could not send response: %s\n", strerror(errno));
                continue;
            }
        }
    }

    close(client_fd);
}
