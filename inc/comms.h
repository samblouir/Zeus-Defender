#ifndef COMMS_H
#define COMMS_H

#include "error.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

xmlDocPtr readxml(int receiver_fd); // Reads XML data from the receiver
int start_uds_listener(); // Starts UDS Listener and returns file descriptor

#endif
