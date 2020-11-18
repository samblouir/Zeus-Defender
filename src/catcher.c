#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "comms.h"
#include "error.h"

// Main function
int main(int argc, char **argv) {
    NPResult result = NP_SUCCESS;
    int fd = 0;
    int port = 0;

    // Check arguments
    if(argc != 3) {
        printf("Usage: ./receiver <Pitcher's IP address> <PORT>\n");
        result = NP_FAIL;
        goto end;
    }
    sscanf(argv[2], "%d", &port);

    // Create a server for listening
    fd = create_server(argv[1], port);
    if(fd < 0) {
        result = NP_FAIL;
        goto end;
    }

    // Receive the XML data
    recv_xml(fd);

end:
    if(result != NP_SUCCESS) {
        print_err(result, "main()");
    }
    return result;
}
