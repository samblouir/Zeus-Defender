#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

/*
 * Must be compiled using:
 * gcc `xml2-config --cflags` -g -Wall -o pitcher pitcher.c `xml2-config --libs`
 */

#ifdef LIBXML_TREE_ENABLED

// Given a packet node and a new root, this will add all of the packet node's children.
static void add_children(xmlNode *packet, xmlNode *new_root) {
    xmlNode *cur_node = NULL;
    xmlNode *to_add = NULL;

    for(cur_node = packet->children; cur_node; cur_node = cur_node->next) {
        to_add = xmlCopyNode(cur_node, 1); // Recursively creates a copy of a child node
        xmlAddChild(new_root, to_add); // Adds the copied child node to the new root
    }
}

// Loops through each node in the PDML file.
// If the node if a packet, then convert the node to a single XML document and print it out.
static void parse_packets(xmlNode *root_node) {
    xmlNode *cur_node = NULL;
    xmlNode *new_root = NULL;
    xmlDocPtr doc = NULL;
    xmlChar *packet_xml_object = NULL;
    int packet_xml_object_size = 0;
    int id = 0;

    // Loop through each node
    for (cur_node = root_node->children; cur_node; cur_node = cur_node->next) {
        // If we see another packet node, then create an XML object
        if (cur_node->type == XML_ELEMENT_NODE && strncmp((const char *) cur_node->name, "packet", 7) == 0) {
            printf("Packet #%d:\n", id);

            // Create a new document and root node for our packet XML object
            doc = xmlNewDoc(BAD_CAST "1.0");
            if(doc == NULL) {
                printf("Error while creating a new XML object.");
                exit(-1);
            }
            new_root = xmlNewNode(NULL, BAD_CAST "packet");
            xmlDocSetRootElement(doc, new_root);
            add_children(cur_node, new_root);

            // Print out the new document
            xmlDocDumpFormatMemoryEnc(doc, &packet_xml_object, &packet_xml_object_size, "UTF-8", 1);
            printf("%s", packet_xml_object);

            // TODO: This is the place where we would call another function to send the XML object to the receiver.

            xmlFree(packet_xml_object);
            packet_xml_object = NULL;

            // Free the document
            xmlFreeDoc(doc);
            doc = NULL;

            // Go to the next packet
            id++;
            printf("\n");
        }
    }
}

// Main function
int main(int argc, char **argv) {
    xmlDoc *doc = NULL;

    if (argc != 2) {
        printf("Requires PDML file for input.\n");
        return 1;
    }

    // Macro that checks whether the libxml2 version in use is compatible with the version that the software has been compiled against.
    // Due to some kind of witchcraft, a semicolon is not necessary here.
    LIBXML_TEST_VERSION

    // Get the document
    doc = xmlReadFile(argv[1], NULL, 0);
    if (doc == NULL) {
        printf("Error while reading the XML document.");
        return -1;
    }

    // Print the elements
    parse_packets(xmlDocGetRootElement(doc));

    // Free the document and cleanup
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}

#else
int main(void) {
    fprintf(stderr, "Tree support not compiled in.\n");
    exit(1);
}
#endif
