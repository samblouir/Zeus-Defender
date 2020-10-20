#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "xml.h"
#include "error.h"

// Main function
int main(int argc, char **argv) {
    xmlDoc *doc = NULL;
    NPResult result = NP_SUCCESS;

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
        result = NP_XML_READ_FILE_ERROR;
        printErr(result, "main()");
    } else {
        // Parse each node
        result = parse_packets(xmlDocGetRootElement(doc));

        // Free the document and cleanup
        xmlFreeDoc(doc);
        xmlCleanupParser();
    }
    return result;
}
