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
        printf("%s\n", xml_str);
    }
}

// Triggered when the receiver has sent a packet to the filter
void packet_available(int receiver2filter) {
    xmlDocPtr packet = NULL;

    // Read data from the receiver to obtain XML data about a single packet.
	packet = receive_xml(receiver2filter);

	// This is just here for testing purposes. You can delete this.
	printxml(packet);

	// Check if the packet is valid
	if(is_valid_packet(packet)) {
		// TODO: Forward the XML data to the flag
	} else {
		// TODO: Print a message saying that a packet was blocked
	}

	// Regardless of whether the packet was allowed or blocked, 
	// send it to the data analytics component. Do not use the schema socket
	// for doing this! Make another UDS socket for communicating with the
	// data analytics component (this will probably also have to be passed
    // in as an input parameter for this function).

	// Delete the packet
	xmlFreeDoc(packet);
}

// Triggered when the data analytics component has sent a new schema
void schema_available(int analytics2filter) {
    // TODO: Read the schema that the data analytics component sent
    // TODO: Update the schema that we are currently using in xml.c
}

int main(void){
    int receiver2filter = 0, analytics2filter = 0;
    struct pollfd fds[2] = {0};

    // Checks whether the libxml2 version is compatible with the software
    // Due to some kind of witchcraft, a semicolon is not necessary here
    LIBXML_TEST_VERSION

    // UDS server sockets for communication
    receiver2filter = receiver_to_filter_socket();
    analytics2filter = analytics_to_filter_socket();

    // Initialize the pollfd structure to listen for the UDS sockets
    fds[0].fd = receiver2filter;
    fds[0].events = POLLIN;
    fds[1].fd = analytics2filter;
    fds[1].fd = POLLIN;

    // Wait for incoming data to arrive at one of the sockets.
	// If something arrives, call the correct function.
    while(poll(fds, 2, -1) >= 0) {
        if(fds[0].revents == POLLIN) {
            packet_available(receiver2filter);
        }

        if(fds[1].revents == POLLIN) {
            schema_available(analytics2filter);
        }
    }
}

    /*
    // Leaving this here for reference

    FILE *fPtr;
    while(1){
        memset(buf, 0, 2048);
        bytes_rec = recv(fd, buf, 2048, 0);
        if (bytes_rec < 0){
            printf("RECV ERROR.\n");
            close(fd);
            break;
        } else {
            printf("DATA RECEIVED = %s\n", buf);
            
            fPtr = fopen("packet.xml", "a");
            fputs(buf, fPtr);
            
           // strcat(str,buf);
        }
    }
    
    //Creating xmlDoc for both the packet and xml schemea 
    xmlDocPtr packetDocPtr;
    xmlDocPtr schemaDocPtr;
    
    //xml filenames
    char * xmlFilename = "packet.xml";
    char * schemaFilename = "schema.xsd";
    
    //creating schema context for validating 
    xmlSchemaParserCtxtPtr schemaParserCtxt;
    xmlSchemaPtr schema;
    xmlSchemaValidCtxtPtr validCtxt;
    
    //error checking
    packetDocPtr = xmlReadFile(xmlFilename, NULL, XML_PARSE_NONET);
    if (!packetDocPtr){
        goto end;
    }
        
    schemaDocPtr = xmlReadFile(schemaFilename, NULL, XML_PARSE_NONET);
    if (!schemaDocPtr){
        goto end;
    }

    schemaParserCtxt = xmlSchemaNewDocParserCtxt(schemaDocPtr);
    if (!schemaParserCtxt){
        goto end;
    }

    schema = xmlSchemaParse(schemaParserCtxt);
    if (!schema){
        goto end;
    }

    validCtxt = xmlSchemaNewValidCtxt(schema);
    if(!validCtxt){
        goto end;
    }

    //XML validation
    xmlSchemaFreeParserCtxt(schemaParserCtxt);
    xmlFreeDoc(schemaDocPtr);

    int valid = 0;
    valid = xmlSchemaValidateDoc(validCtxt, packetDocPtr);
    if (valid == 0){
        printf("Valid Doc \n");
     }else{
       printf("Invalid Doc \n");
    }
    
    //freeing mem
    xmlSchemaFree(schema);
    xmlFreeDoc(packetDocPtr);    
    fclose(fPtr);
    
end: 
    xmlSchemaFree(schema);
    xmlFreeDoc(packetDocPtr);    
    fclose(fPtr);
    return 0;
    */
