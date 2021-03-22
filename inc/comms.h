#ifndef COMMS_H
#define COMMS_H

#include "error.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

#define RECEIVER_TO_FILTER "/tmp/zeus/receiver2filter"
#define ANALYTICS_TO_FILTER "/tmp/zeus/analytics2filter"

xmlDocPtr receive_xml(int fd); // Reads XML data from the given file descriptor
NPResult send_xml(xmlDocPtr doc, int fd); // Sends an XML document to a socket

/* SERVER SOCKETS */
int receiver_to_filter_socket(); // Returns UDS socket for obtaining data from receiver
int analytics_to_filter_socket(); // Returns UDS socket for obtaining data from data analytics

/*
 * Note:
 * You can use something like "nc -nvU /tmp/zeus/analytics2filter" to simulate the
 * data analytics component talking to the filter.
 */

#endif
