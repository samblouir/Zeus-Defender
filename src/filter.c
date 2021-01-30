#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include "comms.h"

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

int main(void){
    int receiver_fd;
    xmlDocPtr doc;

    // Checks whether the libxml2 version is compatible with the software
    // Due to some kind of witchcraft, a semicolon is not necessary here
    LIBXML_TEST_VERSION

    // Start the UDS listener
    receiver_fd = start_uds_listener();

    /* 
     * TODO: We need to figure out a way to have the filter communicate with
     *       the data analytics component. We have two options that I can
     *       think off the top of my head:
     *
     *       1.) Create a new thread, and have that thread communicate with
     *       the data analytics component. If the data analytics component
     *       sends a UDS message to this thread, then this thread will update
     *       the schema. The downside to this is it's a lot more difficult as
     *       we would have to implement code for communicating between threads
     *       as well as communicating between the filter and data analytics
     *       component.
     *
     *       2.) Check whether the data analytics component has sent us a message
     *       right before parsing another packet XML object. In other words, right
     *       after we call readxml() in the while loop, we should call another
     *       function that updates the XML schema we're using before validating the
     *       XML object.
     *
     *       -- Nihaal
     */

    while(1) {
        doc = readxml(receiver_fd);

        // This is just here for testing purposes. You can delete this.
        printxml(doc);

        /*
         * TODO: Create and call another function to validate the schema here.
         *       It should return 1 if valid and 0 otherwise. Preferably,
         *       this function would be defined in a different file. Maybe
         *       something like validate.c or schema.c.
         *
         * TODO: If valid, forward the XML data to the flag. The function that
         *       forwards the data to the flag should be defined in comms.c
         *
         *       -- Nihaal
         */

        xmlFreeDoc(doc);
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
