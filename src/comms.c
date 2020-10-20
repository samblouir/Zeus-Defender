#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "error.h"

// Send an XML document to the receiver
NPResult send_xml(xmlDocPtr doc) {
    NPResult result = NP_SUCCESS;
    xmlChar *xml_str = NULL;
    int xml_str_size = 0;

    // Convert the XML document to a string
    xmlDocDumpFormatMemoryEnc(doc, &xml_str, &xml_str_size, "UTF-8", 1);
    if(xml_str == NULL || xml_str_size == 0) {
        result = NP_XML_TO_STRING_ERROR;
        printErr(result, "send_xml()");
    } else {
        // TODO: Send the string
        printf("%s", xml_str);

        // Free memory
        xmlFree(xml_str);
        xml_str = NULL;
    }

    return result;
}
