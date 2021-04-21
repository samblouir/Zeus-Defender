#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include "comms.h"
#include "xml.h"

// Reads a packet from the receiver, forwards it to the data
// analytics component, waits for the data analytics component
// to return a schema, applies the schema that the data analytics
// component returns to the received packet, and forwards the
// packet to the flag if it is valid.
void process_packet(int receiver2filter, int analytics2filter) {
    int result = 0;
    static int packet_counter = 0;
    static xmlDocPtr prevDocPtr = NULL;
    xmlDocPtr packet = NULL, schemaDocPtr = NULL;
    struct pollfd analytics_poll = {0};

    // Read the packet and send it to the data analytics
    packet = receive_xml(receiver2filter);

    // Skips over everything if we received a null packet
    if (NULL != packet) {
        result = send_xml(packet, analytics2filter);

        while (NP_SUCCESS != result) {
            sleep(1);
            printf("Failed to send to Data Analytics. Retrying...");
            fflush(stdout);
            result = send_xml(packet, analytics2filter);
        }

        // Wait for the data analytics to respond with a schema
        analytics_poll.fd = analytics2filter;
        analytics_poll.events = POLLIN;
        do {
            poll(&analytics_poll, 1, -1);
        } while (!(analytics_poll.revents & POLLIN));
        schemaDocPtr = receive_xml(analytics2filter);
        if(NULL == schemaDocPtr) {
            // If no document was received, then use the previous schemaDocPtr
            schemaDocPtr = prevDocPtr;
        } else {
            // Else, set prevDocPtr to the current XML document so that we can
            // use it next time this function is called
            prevDocPtr = schemaDocPtr;
        }

        printf("Packet #%d:\n", packet_counter);
        printxml(packet);

        // Forward the packet to the flag if it's valid
        if (is_valid_packet(packet, schemaDocPtr)) {
            printf("Packet #%d is deemed to be valid.\n\n", packet_counter);
            send_to_flag(packet, packet_counter);
        } else {
            printf("Packet #%d is deemed to be invalid.\n\n", packet_counter);
        }

        packet_counter++;
        fflush(stdout);
    }
    // Bye bye
    xmlFreeDoc(packet);
    xmlFreeDoc(schemaDocPtr);
}

int main(int argc, char **argv) {
    int receiver2filter = 0, analytics2filter = 0;
    struct pollfd receiver_poll = {0};

    // Checks whether the libxml2 version is compatible with the software
    // Due to some kind of witchcraft, a semicolon is not necessary here
    LIBXML_TEST_VERSION

    // Create necessary directories
    mkdir("/tmp/zeus/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("/tmp/zeus/flag/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    // UDS server sockets for communication
    receiver2filter = receiver_to_filter_socket();
    analytics2filter = analytics_to_filter_socket();

    // Wait until a packet arrives
    receiver_poll.fd = receiver2filter;
    receiver_poll.events = POLLIN;
    while (poll(&receiver_poll, 1, -1) >= 0) {
        if (receiver_poll.revents & POLLIN) {
            process_packet(receiver2filter, analytics2filter);
        }
    }

    printf("\n\nAll done!\n");
    fflush(stdout);

    return 0;
}
