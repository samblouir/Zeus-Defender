#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "heap.h"
#include "error.h"
#include "comms.h"

// Reads a single XML file from the given file descriptor
// Returns the data as an xmlDocPtr
xmlDocPtr receive_xml(int fd) {
    NPResult result = NP_FAIL;
    char *xml = NULL;
    int32_t xml_len = 0;
    int tmp = 0, bytes_counter = 0;
    xmlDocPtr doc = NULL;

    // Get the size of the data
    tmp = recv(fd, &xml_len, sizeof(int32_t), 0);
    if(tmp < 0) {
        result = NP_SOCKET_RECV_MSG_ERROR;
        goto end;
    }
    xml_len = ntohl(xml_len);

    // Read the XML data itself
    xml = my_malloc(xml_len);
    while(bytes_counter < xml_len) {
        tmp = recv(fd, xml+bytes_counter, xml_len-bytes_counter, 0);
        if(tmp < 0) {
            result = NP_SOCKET_RECV_MSG_ERROR;
            goto end;
        }
        bytes_counter += tmp;
    }

    // Convert the string into an xmlDocPtr
    doc = xmlReadMemory(xml, xml_len, "packet.xml", NULL, 0);
    if(NULL == doc) {
        result = NP_XML_DOC_CREATION_ERROR;
        goto end;
    }

    result = NP_SUCCESS;

end:
    if(xml != NULL) {
        free(xml);
        xml = NULL;
    }
    if(NP_SUCCESS != result) {
        print_err(result, "receive_xml()");
    }
    return doc;
}

// Sends an XML document to a socket
NPResult send_xml(xmlDocPtr doc, int fd) {
    NPResult result = NP_FAIL;
    xmlChar *xml = NULL;
    int32_t xml_len = 0;

    // Convert the XML document to a string
    xmlDocDumpFormatMemoryEnc(doc, &xml, &xml_len, "UTF-8", 1);
    if(xml == NULL || xml_len == 0) {
        result = NP_XML_TO_STRING_ERROR;
        goto end;
    }

    // Send the length of the buffer
    xml_len = htonl(xml_len);
    if(send(fd, &xml_len, sizeof(int32_t), 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        goto end;
    }
    xml_len = ntohl(xml_len);

    // Send the data
    if(send(fd, xml, xml_len, 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        goto end;
    }

    result = NP_SUCCESS;

end:
    if(xml != NULL) {
        xmlFree(xml);
        xml = NULL;
    }
    if(NP_SUCCESS != result) {
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
    int server_sock = 0, client_sock = 0, rc = 0;

    printf("INFO: Setting up receiver2filter UDS socket...\n");

    // Create a UDS socket
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1) {
        result = NP_SOCKET_CREATION_ERROR;
        goto end;
    }

    // Setup the UNIX sockaddr structure
    server_sockaddr.sun_family = AF_UNIX;
    strncpy(server_sockaddr.sun_path, RECEIVER_TO_FILTER, strlen(RECEIVER_TO_FILTER) + 1);
    unlink(RECEIVER_TO_FILTER);

    // Bind to the file
    len = sizeof(server_sockaddr);
    rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);
    if (rc < 0) {
        close(server_sock);
        result = NP_SOCKET_BIND_ERROR;
        goto end;
    }

    // Listen for any client sockets
    rc = listen(server_sock, 1);
    if (rc < 0) {
        close(server_sock);
        result = NP_SOCKET_CONNECTION_ERROR;
        goto end;
    }

    // Accept an incoming connection
    client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
    if (client_sock < 0) {
        close(server_sock);
        close(client_sock);
        result = NP_SOCKET_CONNECTION_ERROR;
        goto end;
    }
    result = NP_SUCCESS;

end:
    if (result != NP_SUCCESS) {
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
    int server_sock = 0, client_sock = 0, rc = 0;

    printf("INFO: Setting up analytics2filter UDS socket...\n");

    // Create a UDS socket
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1) {
        result = NP_SOCKET_CREATION_ERROR;
        goto end;
    }

    // Setup the UNIX sockaddr structure
    server_sockaddr.sun_family = AF_UNIX;
    strncpy(server_sockaddr.sun_path, ANALYTICS_TO_FILTER, strlen(ANALYTICS_TO_FILTER) + 1);
    unlink(ANALYTICS_TO_FILTER);

    // Bind to the file
    len = sizeof(server_sockaddr);
    rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);
    if (rc < 0) {
        close(server_sock);
        result = NP_SOCKET_BIND_ERROR;
        goto end;
    }

    // Listen for any client sockets
    rc = listen(server_sock, 1);
    if (rc < 0) {
        close(server_sock);
        result = NP_SOCKET_CONNECTION_ERROR;
        goto end;
    }

    // Accept an incoming connection
    client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
    if (client_sock < 0) {
        close(server_sock);
        close(client_sock);
        result = NP_SOCKET_CONNECTION_ERROR;
        goto end;
    }
    result = NP_SUCCESS;

end:
    if (result != NP_SUCCESS) {
        print_err(result, "analytics_to_filter_socket()");
    } else {
        printf("INFO: analytics2filter socket setup.\n");
    }
    return client_sock;
}
