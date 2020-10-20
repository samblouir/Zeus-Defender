#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "xml.h"
#include "comms.h"
#include "error.h"

// Given a packet node and a new root, this will copy all of the packet node's children to the new root.
static NPResult copy_children(xmlNode *packet, xmlNode *new_root) {
    NPResult result = NP_SUCCESS;
    xmlNode *cur_node = NULL;
    xmlNode *temp = NULL;

    for(cur_node = packet->children; cur_node; cur_node = cur_node->next) {
        temp = xmlCopyNode(cur_node, 1); // Recursively creates a copy of a child node
        if(temp == NULL) { // xmlCopyNode should not return NULL
            result = NP_XML_COPY_ERROR;
        }

        temp = xmlAddChild(new_root, temp); // Adds the copied child node to the new root
        if(temp == NULL) { // xmlAddChild should not return NULL
            result = NP_FAIL;
        }
    }

    if(result != NP_SUCCESS) {
        printErr(result, "add_children()");
    }

    return result;
}

// Loops through each node in the PDML file.
// If the node if a packet, then convert the node to a single XML document and print it out.
NPResult parse_packets(xmlNode *root_node) {
    NPResult result = NP_SUCCESS;
    xmlNode *cur_node = NULL;
    xmlNode *new_root = NULL;
    xmlDocPtr doc = NULL;
    int id = 0;

    // Loop through each node
    for (cur_node = root_node->children; cur_node; cur_node = cur_node->next) {
        // If we see another packet node, then create an XML object
        if (cur_node->type == XML_ELEMENT_NODE
                && strncmp((const char *) cur_node->name, "packet", 7) == 0) {

            printf("Packet #%d:\n", id);

            // Create a new document and root node for our packet XML object
            doc = xmlNewDoc(BAD_CAST "1.0");
            if(doc == NULL) {
                printf("Error while creating a new XML object.");
                exit(-1);
            }
            new_root = xmlNewNode(NULL, BAD_CAST "packet");
            xmlDocSetRootElement(doc, new_root);
            if(xmlDocGetRootElement(doc) != new_root) {
                result = NP_FAIL;
                printErr(result, "parse_packets()");
                break;
            }

            // Copy the children to the new document
            if(copy_children(cur_node, new_root) != NP_SUCCESS) {
                result = NP_FAIL;
                printErr(result, "parse_packets()");
                break;
            }

            // Send the XML object to the receiver.
            if(send_xml(doc) != NP_SUCCESS) {
                result = NP_FAIL;
                printErr(result, "parse_packets()");
                break;
            }

            // Free the document
            xmlFreeDoc(doc);
            doc = NULL;

            // Go to the next packet
            id++;
            printf("\n");
        }
    }

    return result;
}
