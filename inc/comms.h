#ifndef COMMS_H
#define COMMS_H

#include "error.h"

// Returns file descriptor for a UDP socket
int create_socket(char *ip, int port);

// Sends XML data to the given socket
NPResult send_xml(xmlDocPtr doc, int fd);

#endif
