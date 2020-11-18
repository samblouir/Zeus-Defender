#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "error.h"

// Creates the server and returns a file descriptor for the client's socket
int create_server(char *ip, int port) {
    NPResult result = NP_SUCCESS;
    struct sockaddr_in serv_addr = {0};
    struct sockaddr_in client_addr = {0};
    int fd = 0;

    // Create a UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        result = NP_SOCKET_CREATION_ERROR;
        goto end;
    }

    // Fill up the server socket information
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Fill up the client socket information
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip, &(client_addr.sin_addr)) <= 0) {
        result = NP_SOCKET_IP_INVALID_ERROR;
        goto end;
    }

    // Bind the socket with the server address
    if(bind(fd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        result = NP_SOCKET_BIND_ERROR;
        goto end;
    }

end:
    if(result != NP_SUCCESS) {
        print_err(result, "create_server()");
    }
    return fd;
}

// Reads XML data from the pitcher in 2048-byte increments
NPResult recv_xml(int fd) {
    NPResult result = NP_SUCCESS;
    char buf[2048] = {0};

    // TODO: Find a way to break out of this while loop
    // TODO: Forward the data to the filter via UDS instead of just printing it out
    while(1) {
        if(recv(fd, buf, 2048, 0) < 0) {
            result = NP_SOCKET_RECV_MSG_ERROR;
            goto end;
        }
        printf("%s", buf);
    }

end:
    if(result != NP_SUCCESS) {
        print_err(result, "recv_xml()");
    }
    return result;
}
