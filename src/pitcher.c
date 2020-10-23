#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <unistd.h>
#include "xml.h"
#include "comms.h"
#include "error.h"

// Main function
int main(int argc, char **argv) {
    NPResult result = NP_SUCCESS;
    xmlDoc *doc = NULL;
    int port = 0;
    int fd = 0;

    // Check arguments
    if (argc != 4) {
        printf("Usage: ./pitcher <PDML file> <Reciever's IP address> <Receiver's PORT>\n");
        return 1;
    }
    sscanf(argv[3], "%d", &port);

    // Checks whether the libxml2 version is compatible with the software.
    // Due to some kind of witchcraft, a semicolon is not necessary here.
    LIBXML_TEST_VERSION

    // Get a file descriptor for the socket that will talk to the receiver.
    fd = create_socket(argv[2], port);
    if(fd < 0) {
        result = NP_FAIL;
        goto end;
    }

    // Get the document
    doc = xmlReadFile(argv[1], NULL, 0);
    if (doc == NULL) {
        result = NP_XML_READ_FILE_ERROR;
        goto end;
    }

    // Parse each node, and then write each node to the socket
    result = parse_packets(xmlDocGetRootElement(doc), fd);

    // Cleanup
    shutdown(fd, SHUT_RDWR);
    xmlFreeDoc(doc);
    xmlCleanupParser();

end:
    if(result != NP_SUCCESS) {
        print_err(result, "main()");
    }
    return result;
}
