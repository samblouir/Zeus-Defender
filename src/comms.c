#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "error.h"
#include "comms.h"
#include "heap.h"

// Returns a file descriptor for a TCP socket that sends data to the receiver.
// Requires an IP address and port number as input parameters.
int create_socket(char *ip, int port) {
    int fd = 0; // File descriptor to return
    struct sockaddr_in serv_addr = {0}; // Contains information about socket

    // Create a UDP socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        print_err(NP_SOCKET_CREATION_ERROR, "send_xml()");
        goto end;
    }

    // Create the server socket information
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip, &(serv_addr.sin_addr)) <= 0) {
        print_err(NP_SOCKET_IP_INVALID_ERROR, "send_xml()");
        goto end;
    }

    // Connect to the server
    if(connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        print_err(NP_SOCKET_CONNECTION_ERROR, "send_xml()");
        goto end;
    }

end:
    return fd;
}

// Send an XML document to the receiver.
// A socket's file descriptor should be given as an input parameter.
NPResult send_xml(xmlDocPtr doc, int fd) {
    NPResult result = NP_SUCCESS;
    xmlChar *xml_str = NULL;
    int32_t xml_str_len = 0;
    char *buf = NULL;

    // Convert the XML document to a string
    xmlDocDumpFormatMemoryEnc(doc, &xml_str, &xml_str_len, "UTF-8", 1);
    if(xml_str == NULL || xml_str_len == 0) {
        result = NP_XML_TO_STRING_ERROR;
        goto end;
    }

    // Send the length of the buffer
    xml_str_len = htonl(xml_str_len);
    if(send(fd, &xml_str_len, sizeof(int32_t), 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        goto end;
    }
    xml_str_len = ntohl(xml_str_len);

    // Send the buffer
    if(send(fd, xml_str, xml_str_len, 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        goto end;
    }

end:
    if(buf != NULL) {
        free(buf);
        buf = NULL;
    }
    if(xml_str != NULL) {
        xmlFree(xml_str);
        xml_str = NULL;
    }
    if(result != NP_SUCCESS) {
        print_err(result, "send_xml()");
    }
    return result;
}
