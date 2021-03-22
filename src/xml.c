#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include "error.h"
#include "xml.h"

// Returns 1 if the packet adheres to the given rules.
// Returns 0 otherwise.
// Packet must be given as a parameter in XML format.
char is_valid_packet(xmlDocPtr packet, xmlDocPtr schemaDocPtr) {
    NPResult result = NP_FAIL;
    xmlSchemaParserCtxtPtr schemaParserCtxt = NULL;
    xmlSchemaPtr schema = NULL;
    xmlSchemaValidCtxtPtr validCtxt = NULL;
    char ret = 0;

    // Create the XML schema parse context
    schemaParserCtxt = xmlSchemaNewDocParserCtxt(schemaDocPtr);
    if(!schemaParserCtxt) {
        result = NP_XML_SCHEMA_ERROR;
        goto end;
    }

    // Create the XML schema pointer
    schema = xmlSchemaParse(schemaParserCtxt);
    if(!schema) {
        result = NP_XML_SCHEMA_ERROR;
        goto end;
    }

    // Create the XML valid context pointer
    validCtxt = xmlSchemaNewValidCtxt(schema);
    if(!validCtxt) {
        result = NP_XML_SCHEMA_ERROR;
        goto end;
    }

    // Validate the packet
    xmlSchemaFreeParserCtxt(schemaParserCtxt);
    ret = !xmlSchemaValidateDoc(validCtxt, packet);
    result = NP_SUCCESS;

end:
    xmlSchemaFree(schema);
    if(result != NP_SUCCESS) {
        print_err(result, "is_valid_packet()");
    }
    return ret;
}

// Prints out the XML data.
// This is just here for debugging purposes.
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
