#ifndef COMMS_H
#define COMMS_H

#include "error.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

#define RECEIVER_TO_FILTER "/tmp/zeus/receiver2filter"
#define ANALYTICS_TO_FILTER "/tmp/zeus/analytics2filter"

xmlDocPtr receive_xml(int receiver_fd); // Reads XML data from the receiver

/* SERVER SOCKETS */
int receiver_to_filter_socket(); // Returns UDS socket for obtaining data from receiver
int analytics_to_filter_socket(); // Returns UDS socket for obtaining data from data analytics

#endif
