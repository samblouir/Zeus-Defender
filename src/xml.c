#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "xml.h"
#include "comms.h"
#include "error.h"

// Takes a node (which represents a single packet) as a parameter.
// Returns a new XML document that only contains that packet object.
static xmlDocPtr new_packet_xml_doc(xmlNode *packet) {
    xmlDocPtr doc = NULL;
    xmlNode *new_root = NULL;

    // Create a new document
    doc = xmlNewDoc(BAD_CAST "1.0");
    if(doc == NULL) {
        print_err(NP_XML_DOC_CREATION_ERROR, "new_packet_xml_doc()");
        goto end;
    }

    // Create a new root element for the document
    //new_root = xmlNewNode(NULL, BAD_CAST "packet");
    new_root = xmlCopyNode(packet, 1);
    xmlDocSetRootElement(doc, new_root);
    if(xmlDocGetRootElement(doc) != new_root) {
        print_err(NP_XML_NEW_ROOT_CREATION_ERROR, "new_packet_xml_doc()");
        goto end;
    }

end:
    return doc;
}

// Loops through each node in the PDML file.
// Converts the packet nodes into a single XML document.
NPResult parse_packets(xmlNode *root_node, int fd) {
    NPResult result = NP_SUCCESS;
    xmlNode *cur_node = NULL;
    xmlDocPtr doc = NULL;

    // Loop through each node
    for (cur_node = root_node->children; cur_node; cur_node = cur_node->next) {
        // If we see another packet node, then create an XML object
        if (cur_node->type == XML_ELEMENT_NODE
                && strncmp((const char *) cur_node->name, "packet", 7) == 0) {

            // Create a new XML document for the packet
            doc = new_packet_xml_doc(cur_node);
            if(doc == NULL) {
                result = NP_FAIL;
                goto end;
            }

            // Send the XML object to the receiver.
            if(send_xml(doc, fd) != NP_SUCCESS) {
                result = NP_FAIL;
                goto end;
            }

            // Free the document
            xmlFreeDoc(doc);
            doc = NULL;
        }
    }

end:
    if(result != NP_SUCCESS) {
        print_err(result, "parse_packets()");
    }
    return result;
}
