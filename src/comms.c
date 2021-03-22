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
#include "heap.h"
#include "error.h"
#include "comms.h"

// Reads a single XML file from the given file descriptor.
// Returns the data into as an xmlDocPtr.
xmlDocPtr receive_xml(int fd) {
    NPResult result = NP_FAIL;
    char *buf = NULL;
    size_t len = 0;
    xmlDocPtr doc = NULL;

    // Get the size of the data
    buf = my_malloc(8);
    if(recv(fd, buf, 8, 0) < 0) {
        result = NP_SOCKET_RECV_MSG_ERROR;
        goto end;
    }
    sscanf(buf, "%lu", &len);
    free(buf);
    buf = NULL;

    // Read the XML data itself
    buf = my_malloc(len);
    if(recv(fd, buf, len, 0) < 0) {
        result = NP_SOCKET_RECV_MSG_ERROR;
        goto end;
    }

    // Convert the string into an xmlDocPtr
    doc = xmlReadMemory(buf, len, "packet.xml", NULL, 0);
    if(doc == NULL) {
        result = NP_XML_DOC_CREATION_ERROR;
        goto end;
    }

    result = NP_SUCCESS;

end:
    if(buf != NULL) {
        free(buf);
        buf = NULL;
    }
    if(result != NP_SUCCESS) {
        print_err(result, "receive_xml()");
    }
    return doc;
}

// Sends an XML document to a socket
NPResult send_xml(xmlDocPtr doc, int fd) {
    NPResult result = NP_SUCCESS;
    xmlChar *xml_str = NULL;
    char xml_str_len_buf[8] = {0};
    int xml_str_len = 0;

    // Convert the XML document to a string
    xmlDocDumpFormatMemoryEnc(doc, &xml_str, &xml_str_len, "UTF-8", 1);
    if(xml_str == NULL || xml_str_len == 0) {
        result = NP_XML_TO_STRING_ERROR;
        goto end;
    }

    // Send the length of the string
    snprintf(xml_str_len_buf, 8, "%lu", (size_t) xml_str_len);
    if(send(fd, xml_str_len_buf, 8, 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        goto end;
    }

    // Send the string
    if(send(fd, xml_str, xml_str_len, 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        goto end;
    }

end:
    if(xml_str != NULL) {
        xmlFree(xml_str);
    }
    if(result != NP_SUCCESS) {
        print_err(result, "send_xml()");
    }
    return result;
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

    printf("INFO: Setting up receiver2filter UDS socket...\n");

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
        print_err(result, "receiver_to_filter_socket()");
    } else {
        printf("INFO: receiver2filter socket setup.\n");
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

    printf("INFO: Setting up analytics2filter UDS socket...\n");

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
        print_err(result, "analytics_to_filter_socket()");
    } else {
        printf("INFO: analytics2filter socket setup.\n");
    }
    return client_sock;
}
