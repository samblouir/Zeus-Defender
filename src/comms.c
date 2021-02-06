#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "error.h"
#include "comms.h"

// Reads a single XML file from the receiver.
// Returns the data into as an xmlDocPtr.
// Takes the file descriptor for receivedpackets as a parameter.
xmlDocPtr receive_xml(int receiver_fd) {
    NPResult result = NP_FAIL;
    char buf[2048] = {0};
    int bytes_read = 0;
    xmlDocPtr doc = NULL;

    bytes_read = recv(receiver_fd, buf, sizeof(buf), 0);
    if(bytes_read == -1) {
        result = NP_SOCKET_RECV_MSG_ERROR;
        goto end;
    }

    // Convert the string into an xmlDocPtr
    doc = xmlReadMemory(buf, bytes_read, "packet.xml", NULL, 0);
    if(doc == NULL) {
        result = NP_XML_DOC_CREATION_ERROR;
        goto end;
    }

    result = NP_SUCCESS;

end:
    if(result != NP_SUCCESS) {
        print_err(result, "receive_xml()");
    }
    return doc;
}

// Creates a UDS listener for the receiver can connect to.
// Waits for the receiver to connect and returns a file
// descriptor for the UDS socket.
int receiver_to_filter_socket() {
    NPResult result = NP_FAIL;
    struct sockaddr_un server_sockaddr = {0};
    struct sockaddr_un client_sockaddr = {0};
    unsigned int len = 0;
    int server_sock=0, client_sock=0, rc=0;

    // Create a UDS socket
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_sock == -1) {
        result = NP_SOCKET_CREATION_ERROR;
        goto end;
    }

    // Setup the UNIX sockaddr structure
    server_sockaddr.sun_family = AF_UNIX;
    strncpy(server_sockaddr.sun_path, RECEIVER_TO_FILTER, strlen(RECEIVER_TO_FILTER)+1);
    unlink(RECEIVER_TO_FILTER);
    
    // Bind to the file
    len = sizeof(server_sockaddr);
    rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);
    if(rc < 0) {
        close(server_sock);
        result = NP_SOCKET_BIND_ERROR;
        goto end;
    }

    // Listen for any client sockets
    rc = listen(server_sock, 1);
    if(rc < 0) {
        close(server_sock);
        result = NP_SOCKET_CONNECTION_ERROR;
        goto end;
    }

    // Accept an incoming connection
    client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
    if(client_sock < 0) {
        close(server_sock);
        close(client_sock);
        result = NP_SOCKET_CONNECTION_ERROR;
        goto end;
    }
    result = NP_SUCCESS;

end:
    if(result != NP_SUCCESS) {
        print_err(result, "start_receivedpackets_listener()");
    }
    return client_sock;
}

// Creates a UDS listener for the data analytics component.
// Waits for the data analytics component to connect and
// returns a file descriptor for the UDS socket.
int analytics_to_filter_socket() {
    NPResult result = NP_FAIL;
    struct sockaddr_un server_sockaddr = {0};
    struct sockaddr_un client_sockaddr = {0};
    unsigned int len = 0;
    int server_sock=0, client_sock=0, rc=0;

    // Create a UDS socket
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_sock == -1) {
        result = NP_SOCKET_CREATION_ERROR;
        goto end;
    }

    // Setup the UNIX sockaddr structure
    server_sockaddr.sun_family = AF_UNIX;
    strncpy(server_sockaddr.sun_path, ANALYTICS_TO_FILTER, strlen(ANALYTICS_TO_FILTER)+1);
    unlink(ANALYTICS_TO_FILTER);
    
    // Bind to the file
    len = sizeof(server_sockaddr);
    rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);
    if(rc < 0) {
        close(server_sock);
        result = NP_SOCKET_BIND_ERROR;
        goto end;
    }

    // Listen for any client sockets
    rc = listen(server_sock, 1);
    if(rc < 0) {
        close(server_sock);
        result = NP_SOCKET_CONNECTION_ERROR;
        goto end;
    }

    // Accept an incoming connection
    client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
    if(client_sock < 0) {
        close(server_sock);
        close(client_sock);
        result = NP_SOCKET_CONNECTION_ERROR;
        goto end;
    }
    result = NP_SUCCESS;

end:
    if(result != NP_SUCCESS) {
        print_err(result, "start_schema_listener()");
    }
    return client_sock;
}
