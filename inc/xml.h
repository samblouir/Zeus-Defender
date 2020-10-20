#ifndef XML_C
#define XML_C

#ifdef LIBXML_TREE_ENABLED

#include "error.h"

// Copies each packet to a new document and calls send_xml() on each of them
NPResult parse_packets(xmlNode *root_node, int fd);

#endif
#endif
