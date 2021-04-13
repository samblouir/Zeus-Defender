#ifndef COMMS_H
#define COMM_H

#include "error.h"

// Creates the server and returns a file descriptor for the client's socket
int create_server(char *ip, int port);

// Returns a file descriptor for the UDS client
int create_uds_client(char *path);

// Reads XML data from the pitcher
NPResult recv_xml(int fd, int filter);

// Forwards the data to the filter via UDS
NPResult forward_to_filter(int filter, char *xml, int32_t xml_len);

#endif
