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
    int temp = recv(fd, buf, 8, 0);
    if (temp < 0) {
        printf("\nNP_SOCKET_RECV_MSG_ERROR in receive_xml(), temp == %d...\n", temp);
        result = NP_SOCKET_RECV_MSG_ERROR;
        goto end;
    }
    sscanf(buf, "%lu", &len);
    printf("filter: len (buf_len) == %lu\n", len); // Always prints 0?
    free(buf);
    buf = NULL;

    // Read the XML data itself
    buf = my_malloc(len);

    size_t total_received = 0;
    size_t curr = 0;

    while (total_received < len) {
        curr = recv(fd, buf, len, 0);
        total_received += curr;

        if (-1 == curr)
            break;

        if (total_received > len) {
            printf("total received > len...?, total_received/len == %zu / %zu\n", total_received, len);
            result = NP_SOCKET_RECV_MSG_ERROR;
            goto end;
        }
    }

//    if (temp < 0) {
//        printf("\nNP_SOCKET_RECV_MSG_ERROR in receive_xml(), temp == %d...\n", temp);
//        result = NP_SOCKET_RECV_MSG_ERROR;
//        goto end;
//    }

    // Convert the string into an xmlDocPtr
    doc = xmlReadMemory(buf, len, "packet.xml", NULL, 0);
    if (NULL == doc) {
        printf("\nNP_XML_DOC_CREATION_ERROR in receive_xml()...\n");
        printf("\nlen == %zu...\n", len);
        result = NP_XML_DOC_CREATION_ERROR;
        goto end;
    }

    result = NP_SUCCESS;

    end:
    if (NULL != buf) {
        free(buf);
        buf = NULL;
    }
    if (NP_SUCCESS != result) {
        print_err(result, "receive_xml()");
        printf("Failure in receive_xml()...\n");
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
    if (xml_str == NULL || xml_str_len == 0) {
        result = NP_XML_TO_STRING_ERROR;
        goto end;
    }

    // Send the length of the string
    snprintf(xml_str_len_buf, 8, "%lu", (size_t) xml_str_len);
    if (send(fd, xml_str_len_buf, 8, 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        goto end;
    }

    // Send the string
    if (send(fd, xml_str, xml_str_len, 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        goto end;
    }

    end:
    if (xml_str != NULL) {
        xmlFree(xml_str);
    }
    if (result != NP_SUCCESS) {
        print_err(result, "send_xml()");
        printf("Error in send_xml()!\n");
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
