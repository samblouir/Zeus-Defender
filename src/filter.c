#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/poll.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include "comms.h"
#include "xml.h"

// Prints out the XML data.
// This is just here for testing purposes.
// You can delete this later.
void printxml(xmlDocPtr doc) {
    xmlChar *xml_str = NULL;
    int xml_str_len = 0;
    xmlDocDumpFormatMemoryEnc(doc, &xml_str, &xml_str_len, "UTF-8", 1);
    if(xml_str == NULL || xml_str_len == 0) {
        print_err(NP_XML_TO_STRING_ERROR, "printxml()");
        exit(0);
    } else {
        printf("XML data:\n");
        printf("%s\n", xml_str);
    }
}

// Triggered when the receiver has sent a packet to the filter
void packet_available(int receiver2filter, xmlDocPtr schemaDocPtr) {
    xmlDocPtr packet = NULL;

    // Read data from the receiver to obtain XML data about a single packet.
	packet = receive_xml(receiver2filter);

	// Check if the packet is valid
	if(is_valid_packet(packet, schemaDocPtr)) {
		// TODO: Forward the XML data to the flag
        printxml(packet);
	} else {
        printf("Invalid packet has been blocked.\n");
	}

	// TODO: Regardless of whether the packet was allowed or blocked, 
	// send it to the data analytics component. Do not use the schema socket
	// for doing this! Make another UDS socket for communicating with the
	// data analytics component (this will probably also have to be passed
    // in as an input parameter for this function).

	// Delete the packet
	xmlFreeDoc(packet);
}

int main(int argc, char **argv){
    int receiver2filter = 0, analytics2filter = 0;
    struct pollfd fds[2] = {0};
    xmlDocPtr schemaDocPtr = NULL;

    // Checks whether the libxml2 version is compatible with the software
    // Due to some kind of witchcraft, a semicolon is not necessary here
    LIBXML_TEST_VERSION

    // Initialize the XML schema
    schemaDocPtr = default_schema();

    // UDS server sockets for communication
    receiver2filter = receiver_to_filter_socket();
    analytics2filter = analytics_to_filter_socket();

    // Initialize the pollfd structure to listen for the UDS sockets
    fds[0].fd = receiver2filter;
    fds[0].events = POLLIN;
    fds[1].fd = analytics2filter;
    fds[1].events = POLLIN;

    // Wait for incoming data to arrive at one of the sockets.
	// If something arrives, call the correct function.
    while(poll(fds, 2, -1) >= 0) {
        if(fds[1].revents == POLLIN) {
            printf("HELLO\n");
            xmlFreeDoc(schemaDocPtr);
            schemaDocPtr = receive_xml(analytics2filter);
        }
        if(fds[0].revents == POLLIN) {
            packet_available(receiver2filter, schemaDocPtr);
        }
    }
}
