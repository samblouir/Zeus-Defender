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
    int server_fd = 0, client_fd = 0, filter = 0;
    int port = 0;

    // Check arguments
    if(argc != 3) {
        printf("Usage: ./receiver <pitcher's IP> <port>\n");
        goto end;
    }
    sscanf(argv[2], "%d", &port);

    // Create a server for listening
	printf("INFO: Creating TCP server for listening...\n");
    server_fd = create_server(argv[1], port);
    if(server_fd < 0) {
        result = NP_FAIL;
        goto end;
    }

    // Initialize the UDS client that connects to the filter
	printf("INFO: Connecting to /tmp/zeus/receiver2filter...\n");
    filter = create_uds_client("/tmp/zeus/receiver2filter");
    if(filter < 0) {
        result = NP_FAIL;
        goto end;
    }

    // Receive XML data forever
    while(1) {
        printf("INFO: Waiting for pitcher\'s connection...\n");
        client_fd = accept(server_fd, NULL, NULL);
        printf("INFO: Accepted connection from pitcher.\n");
        recv_xml(client_fd, filter);
        printf("INFO: Closed connection to pitcher.\n");
    }

    result = NP_SUCCESS;

end:
    if(result != NP_SUCCESS) {
        print_err(result, "main()");
    }
    return result;
}
