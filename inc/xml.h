#ifndef XML_H
#define XML_H

#define DEFAULT_SCHEMA_FILE "schema.xsd"

// Returns 1 if the given packet is valid, and 0 otherwise
// Uses the given XML schema document
char is_valid_packet(xmlDocPtr packet, xmlDocPtr schemaDocPtr);

// Prints out XML data
void printxml(xmlDocPtr doc);

#endif
