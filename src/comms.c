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
        printf("ERROR: ");
        print_err(result, "create_server()");
        printf(" (%s)\n", strerror(errno));
        // printf("ERROR: %s\n", strerror(errno));
    }
    return fd;
}

int create_uds_client(char *path) {
    int fd = 0; // File descriptor to return
    struct sockaddr_un serv_addr = {0}; // Contains information about socket

    // Create a UDS socket
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("ERROR: Couldn't create socket (%s)\n", strerror(errno));
        return -1;
    }
    printf("INFO: Created uds client socket { fd = %d }\n", fd);

    // Create the server socket information
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, path, sizeof(serv_addr.sun_path));
    printf("INFO: Created sockaddr with path %s\n", serv_addr.sun_path);


    // Connect to the server
    if (connect(fd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("ERROR: Couldn't connect socket to address %s (%s)\n", serv_addr.sun_path, strerror(errno));
        return -1;
    }
    printf("INFO: Connected to address %s (%s)\n", serv_addr.sun_path, strerror(errno));
goto end;
end:
    return fd;

}

// Reads XML data from the pitcher in 2048-byte increments
NPResult recv_xml(int fd) {
    NPResult result = NP_SUCCESS;
    char *buf = NULL;
    char buf_len_buf[8] = {0};
    size_t buf_len = 0;
    int running = 1;
    int res = 0;
    int filter = create_uds_client("/tmp/zeus/receiver2filter");

    printf("INFO: filter: %d\n", filter);
    while (filter < 0) {
        printf("Failed in create_uds_client(\"...\"). Retrying...");
        sleep(1);
        filter = create_uds_client("/tmp/zeus/receiver2filter");
        // result = NP_FAIL;
        // goto end;
    }

    int packet_counter = 0;
    while (running) {
        // Obtain the length of the XML data
        memset(buf_len_buf, 0, 8);
        if (recv(fd, buf_len_buf, 8, 0) < 0) {
            result = NP_SOCKET_RECV_MSG_ERROR;
            goto end;
        }
<<<<<<< HEAD
        printf("%s", buf);
        int res = forward_to_filter(filter, buf);
        if(res < 0) {
=======
        sscanf(buf_len_buf, "%lu", &buf_len);
        printf("receiver: packet #%d, buf_len == %lu\n", packet_counter++, buf_len);

        // Obtain the XML data
        buf = my_malloc(buf_len);
        if (recv(fd, buf, buf_len, 0) < 0) {
            result = NP_SOCKET_RECV_MSG_ERROR;
            goto end;
        }

        // Forward the XML data
        res = forward_to_filter(filter, buf, buf_len, buf_len_buf);
        free(buf);
        buf = NULL;
        if (res < 0) {
>>>>>>> 7a309d02faa4f35508fd940bcd04db6f1a129978
            result = NP_FAIL;
            goto end;
        }
    }

    end:
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }
    if (result != NP_SUCCESS) {
        print_err(result, "recv_xml()");
    }
    return result;
}

NPResult forward_to_filter(int filter, const char *data, size_t len, const char *data_len_buf) {
    NPResult result = NP_SUCCESS;

    if (data == NULL) {
        result = NP_FAIL;
        goto end;
    }

    // Send the length of the data
    if (send(filter, data_len_buf, 8, 0) < 0) {
        result = NP_FAIL;
        goto end;
    }

    // Send the data itself
    if (send(filter, data, len, 0) < 0) {
        result = NP_FAIL;
        goto end;
    }

    end:
    if (result != NP_SUCCESS) {
        print_err(result, "forward_to_filter()");
    }
    return result;
}
