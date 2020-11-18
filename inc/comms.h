#ifndef COMMS_H
#define COMM_H

#include "error.h"

// Creates the server and returns a file descriptor for the client's socket
int create_server(char *ip, int port);

// Reads XML data from the pitcher in 2048-byte increments
NPResult recv_xml(int fd);

#endif
