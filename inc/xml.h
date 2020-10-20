#ifndef XML_C
#define XML_C

#ifdef LIBXML_TREE_ENABLED

void add_children(xmlNode *packet, xmlNode *new_root);
void parse_packets(xmlNode *root_node);

#endif
#endif
