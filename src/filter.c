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

static int packet_counter = 0;

// Reads a packet from the receiver, forwards it to the data
// analytics component, waits for the data analytics component
// to return a schema, applies the schema that the data analytics
// component returns to the received packet, and forwards the
// packet to the flag if it is valid.
void process_packet(int receiver2filter, int analytics2filter) {
//    printf("\n\nEntering process_packet(%d, %d)", receiver2filter, analytics2filter);
//    fflush(stdout);

    int result = 0;
    xmlDocPtr packet = NULL, schemaDocPtr = NULL;
    struct pollfd analytics_poll = {0};

    // Read the packet and send it to the data analytics
    packet = receive_xml(receiver2filter);

    // Skips over everything if we received a null packet
    if (packet != NULL) {


//        // Retry empty packets
//        while (packet == NULL) {
//            printf("Failed to receive from the receiver. Retrying...\n");
//            fflush(stdout);
//            sleep(1);
//            packet = receive_xml(receiver2filter);
//        }



//    printf("\n\n");
//    printf("*************************\n");
//    printf("Sending packet to data analytics...\n\n");
//    fflush(stdout);
        result = send_xml(packet, analytics2filter);

        while (result != NP_SUCCESS) {
            sleep(1);
            printf("Failed to send to Data Analytics. Retrying...");
            fflush(stdout);
            result = send_xml(packet, analytics2filter);
        }
//    printf("\nSuccessfully sent packet!\n");
//    printf("*************************\n");
//    fflush(stdout);

        // Wait for the data analytics to respond with a schema
        analytics_poll.fd = analytics2filter;
        analytics_poll.events = POLLIN;
        do {
            poll(&analytics_poll, 1, -1);
        } while (!(analytics_poll.revents & POLLIN));
        schemaDocPtr = receive_xml(analytics2filter);

        while (schemaDocPtr == NULL) {
            printf("Failed to receive from Data Analytics. Retrying...");
            fflush(stdout);
            sleep(1);
            schemaDocPtr = receive_xml(analytics2filter);
        }

        // Forward the packet to the flag if it's valid
        if (is_valid_packet(packet, schemaDocPtr)) {
            printxml(packet);
            // TODO: Replace the above line with a call to send_xml(),
            // which will forward the packet to the flag
        }

        printf("\n\n  Finished in process_packet(%d, %d) (#%d)", receiver2filter, analytics2filter, packet_counter);
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
        printf("\n\tStep BBB\n");
        fflush(stdout);
    }

    printf("\n\nAll done!\n");
    fflush(stdout);

    return 0;
}
