#!/usr/bin/python3
import socket
import sys
import os

# Socket address to connect to
ANALYTICS_TO_FILTER = '/tmp/zeus/analytics2filter'

# Creates a socket to connect to the filter and returns it.
# This should be called once at the beginning of the program.
# Note that the filter will wait until the DA connects to it, so.
# if you don't call this function, the filter will be waiting forever.
# Don't forget to close the socket later.
def analytics_to_filter_socket():
    # Create a UDS socket and attempt to connect to it
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    try:
        sock.connect(ANALYTICS_TO_FILTER)
    except socket.error as msg:
        print(msg)
        sys.exit(1)

    return sock

# Used when receiving XML data.
# Currently only returns a string,
# but you could change it to return an XML object instead.
def recv_xml(sock):
    try:
        # The first four bytes represent the size of the data
        size = int.from_bytes(sock.recv(4), "big")

        # Get the XML data itself
        data = sock.recv(size).decode('utf-8') # Data is returned as a string
        return data # TODO: Convert from string to XML
    except Exception as e:
        print(e)
        sys.exit(1)

# Used to send XML data.
# Currently, requires data parameter to be a string,
# but you could change it to require data to be an XML object instead.
# Can also be used to send XML schemas (because schemas are written in xml).
def send_xml(data, sock):
    # TODO: Make this work even when data is not a string
    try:
        sock.send(socket.htonl(len(data)).to_bytes(4, "little"))
        sock.send(data.encode('utf-8'))
    except Exception as e:
        print(e)
        sys.exit(1)

# Example: Sending three XML schemas, one that allows the first packet and
# another that allows the middle packet.
if(__name__ == "__main__" and len(sys.argv) == 2 and sys.argv[1] == "test"):
    analytics2filter = analytics_to_filter_socket()
    with open('schema_allow_first.xsd', 'r') as f:
        schema_str = f.read()
        print(recv_xml(analytics2filter))
        send_xml(schema_str, analytics2filter)
    with open('schema_allow_last.xsd', 'r') as f:
        schema_str = f.read()
        print(recv_xml(analytics2filter))
        send_xml(schema_str, analytics2filter)
        print(recv_xml(analytics2filter))
        send_xml(schema_str, analytics2filter)
    analytics2filter.close()
    exit(0)
    
    
# Example: Read a packet from the filter and send an XML schema to the filter.
# The XML schema filename should be given as an argument
if(__name__ == "__main__" and len(sys.argv) == 2):
    analytics2filter = analytics_to_filter_socket()
    with open(sys.argv[1], 'r') as f:
        print(recv_xml(analytics2filter))
        schema_str = f.read()
        send_xml(schema_str, analytics2filter)
    analytics2filter.close()
      

# Same as above, but does it in a loop
if(__name__ == "__main__" and len(sys.argv) == 3):
    analytics2filter = analytics_to_filter_socket()
    with open(sys.argv[1], 'r') as f:
        schema_str = f.read()
        while(True):
            print(recv_xml(analytics2filter))
            send_xml(schema_str, analytics2filter)
    analytics2filter.close()


"""
# Example: Sending three XML schemas, one that allows the first packet and
# another that allows the middle packet.
if(__name__ == "__main__"):
    analytics2filter = analytics_to_filter_socket()
    with open('schema_allow_first.xsd', 'r') as f:
        schema_str = f.read()
        print(recv_xml(analytics2filter))
        send_xml(schema_str, analytics2filter)
    with open('schema_allow_last.xsd', 'r') as f:
        schema_str = f.read()
        print(recv_xml(analytics2filter))
        send_xml(schema_str, analytics2filter)
        print(recv_xml(analytics2filter))
        send_xml(schema_str, analytics2filter)
    analytics2filter.close()
"""
