#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <errno.h>
#include "error.h"
#include "heap.h"
#include "comms.h"

#ifndef __DEBUG__
#define __DEBUG__
#endif

#ifdef __DEBUG__
#define DPRINT printf
#else
#define DPRINT
#endif

// Creates the server and returns a file descriptor for the client's socket
int create_server(char *ip, int port) {
    NPResult result = NP_SUCCESS;
    struct sockaddr_in serv_addr = {0};
    struct sockaddr_in client_addr = {0};
    int fd = 0;

    // Create a UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        result = NP_SOCKET_CREATION_ERROR;
        goto end;
    }
    printf("INFO: Created server: socket { fd = %d }\n", fd);

    // Fill up the server socket information
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Fill up the client socket information
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &(client_addr.sin_addr)) <= 0) {
        result = NP_SOCKET_IP_INVALID_ERROR;
        goto end;
    }

    // Bind the socket with the server address
    if (bind(fd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        result = NP_SOCKET_BIND_ERROR;
        goto end;
    }
    printf("INFO: Bound { AF_INET, %d, %d }\n", serv_addr.sin_addr.s_addr, serv_addr.sin_port);

end:
    if (result != NP_SUCCESS) {
        print_err(result, "create_server()");
    }
    return fd;
}

// Returns a file descriptor for the UDS client
int create_uds_client(char *path) {
    NPResult result = NP_FAIL;
    int fd = 0;
    struct sockaddr_un serv_addr = {0};

    // Create a UDS socket
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0) {
        result = NP_SOCKET_CREATION_ERROR;
        goto end;
    }
    printf("INFO: Created UDS client socket { fd = %d }\n", fd);

    // Create the server socket information
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, path, sizeof(serv_addr.sun_path));
    printf("INFO: Created sockaddr with path %s\n", serv_addr.sun_path);

    // Connect to the server
    if(connect(fd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("ERROR: Couldn't connect to address %s (%s)\n",
                serv_addr.sun_path, strerror(errno));
        result = NP_SOCKET_CONNECTION_ERROR;
    }

    printf("INFO: Connected to address %s (%s)\n", serv_addr.sun_path, strerror(errno));
    result = NP_SUCCESS;

end:
    if(result != NP_SUCCESS) {
        print_err(result, "create_uds_client()");
        printf("ERROR: Retrying UDS connection with filter.\n");
        fd = create_uds_client(path);
    }
    return fd;
}

// Reads XML data from the pitcher and forwards it to the filter
NPResult recv_xml(int fd) {
    NPResult result = NP_FAIL;
    char *xml = NULL;
    int32_t xml_len = 0;
    int filter = 0, bytes_counter = 0, running = 1, tmp = 0;
    long packet_counter = 0;

    // Initialize the UDS client that connects to the filter
    filter = create_uds_client("/tmp/zeus/receiver2filter");
    if(filter < 0) {
        result = NP_SOCKET_CREATION_ERROR;
        goto end;
    }

    while (running) {
        // Obtain the length of the data
        tmp = recv(fd, &xml_len, sizeof(int32_t), 0);
        if(tmp < 0) {
            result = NP_SOCKET_RECV_MSG_ERROR;
            goto end;
        }
        xml_len = ntohl(xml_len);
        printf("INFO: Reading %d bytes from the pitcher...\n", xml_len);

        // Obtain the rest of the XML data
        // This is placed in a loop to ensure that every byte is read
        bytes_counter = 0;
        xml = my_malloc(xml_len);
        while(bytes_counter < xml_len) {
            tmp = recv(fd, xml+bytes_counter, xml_len-bytes_counter, 0);
            if(tmp < 0) {
                result = NP_SOCKET_RECV_MSG_ERROR;
                goto end;
            }
            bytes_counter += tmp;
        }
        
        // Forward the data to the filter
        printf("INFO: Received packet #%ld (%d bytes):\n%s\n", packet_counter, xml_len, xml);
        if(forward_to_filter(filter, xml, xml_len) != NP_SUCCESS) {
            result = NP_FAIL;
            goto end;
        }
        free(xml);
        xml = NULL;
        packet_counter++;
    }

    result = NP_SUCCESS;

end:
    if(xml != NULL) {
        free(xml);
        xml = NULL;
    }
    if(result != NP_SUCCESS) {
        print_err(result, "recv_xml()");
    }
    return result;
}

// Forward the data to the filter
NPResult forward_to_filter(int filter, char *xml, int32_t xml_len) {
    NPResult result = NP_FAIL;

    if(xml == NULL || xml_len == 0) {
        result = NP_SUCCESS;
        goto end;
    }

    // Send the length of the buffer
    xml_len = htonl(xml_len);
    if(send(filter, &xml_len, sizeof(int32_t), 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        goto end;
    }
    xml_len = ntohl(xml_len);

    // Send the data
    if(send(filter, xml, xml_len, 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        goto end;
    }

    result = NP_SUCCESS;

end:
    if(result != NP_SUCCESS) {
        print_err(result, "forward_to_filter()");
    }
    return result;
}
