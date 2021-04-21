#ifndef XML_H
#define XML_H

#define DEFAULT_SCHEMA_FILE "schema.xsd"

#include "error.h"

// Returns 1 if the given packet is valid, and 0 otherwise
// Uses the given XML schema document
char is_valid_packet(xmlDocPtr packet, xmlDocPtr schemaDocPtr);

// Prints out XML data
void printxml(xmlDocPtr doc);

// Sends a given XML packet to the flag
NPResult send_to_flag(xmlDocPtr packet, int packet_counter);

#endif
