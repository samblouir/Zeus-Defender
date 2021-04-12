#ifndef COMMS_H
#define COMM_H

#include "error.h"

// Creates the server and returns a file descriptor for the client's socket
int create_server(char *ip, int port);

// Reads XML data from the pitcher in 2048-byte increments
NPResult recv_xml(int fd);

// Forwards the data to the filter via UDS
NPResult forward_to_filter(int filter, char *xml, int32_t xml_len);

#endif
