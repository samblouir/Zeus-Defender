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
char is_valid_packet(xmlDocPtr packet) {
    NPResult result = NP_FAIL;
    char ret = 0;

    // TODO: Validate packet using an XML schema
    // TODO: Set ret to 1 if valid, else set it to 0
    result = NP_SUCCESS;
    ret = 1;

    // I just put this here to avoid a compiler warning about unused labels.
    // You should delete this line later when you actually program this.
    goto end;

end:
    if(result != NP_SUCCESS) {
        print_err(result, "is_valid_packet()");
    }
    return ret;
}
