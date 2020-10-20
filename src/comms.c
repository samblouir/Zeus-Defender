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

// Returns a file descriptor for a UDP socket that sends data to the receiver.
// Requires an IP address and port number as input parameters.
int create_socket(char *ip, int port) {
    int fd = 0; // File descriptor to return
    struct sockaddr_in serv_addr = {0}; // Contains information about socket

    // Create a UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        printErr(NP_SOCKET_CREATION_ERROR, "send_xml()");
        goto end;
    }

    // Create the server socket information
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip, &(serv_addr.sin_addr)) <= 0) {
        printErr(NP_SOCKET_IP_INVALID_ERROR, "send_xml()");
        goto end;
    }

    // Connect to the server
    if(connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printErr(NP_SOCKET_CONNECTION_ERROR, "send_xml()");
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
    int xml_str_len = 0;

    // Convert the XML document to a string
    xmlDocDumpFormatMemoryEnc(doc, &xml_str, &xml_str_len, "UTF-8", 1);
    if(xml_str == NULL || xml_str_len == 0) {
        result = NP_XML_TO_STRING_ERROR;
        printErr(result, "send_xml()");
        goto end;
    }

    // Send the string
    if(send(fd, xml_str, xml_str_len, 0) < 0) {
        result = NP_SOCKET_SEND_MSG_ERROR;
        printErr(result, "send_xml()");
        goto end;
    }

    // Free memory
    xmlFree(xml_str);
    xml_str = NULL;

end:
    return result;
}
