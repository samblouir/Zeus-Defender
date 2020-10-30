# Pitcher

### Usage
1. Setup two VMs, one for the pitcher and another for the receiver.
    1. You can also use `nc -nlvup 4444` on a separate terminal window to act as a test receiver.    
2. Get the location of the XML file that you wish to send.
3. Use `./pitcher <XML file> <Receiver's IP address> <Receiver's PORT>` to send the file.
    1. If you're using the `nc` command, then you can do `./pitcher <XML file> 127.0.0.1 4444`.

### Networking
Networking for this program is handled in src/comms.c. The `main()` function will call `create_socket()` in comms.c, which is where the socket is first created. `create_socket()` will create a file descriptor for a socket by using `socket(AF_INET, SOCK_DGRAM, 0)`, use `inet_pton()` to save the IP address information in `serv_addr.sin_addr`, call `connect()`, and return the file descriptor. When an XML packet object needs to be sent to the receiver, the `send_xml()` function is called, which will use `xmlDocDumpFormatMemoryEnc()` to convert the XML document to a string and send the string using `send()`.

### Parsing XML
Most of the XML parsing code is done in xml.c. The `main()` function will call `xmlReadFile()` to obtain an XML document pointer. Using the XML document pointer as an argument, `main()` will then call `parse_packets()`, located in xml.c. `parse_packets()` will loop through every child node in the given XML document, and if the child's name is "packet," the function will create a new XML document that only contains that packet and call `send_xml()`. It accomplishes this by using a static function called `new_packet_xml_doc()`, which create a copy of the XML document and returns the copy.

# Zeus-Defender

### Basic Setup
1. Download Xubuntu from https://mirror.us.leaseweb.net/ubuntu-cdimage/xubuntu/releases/20.04/release/xubuntu-20.04-desktop-amd64.iso.
2. Setup two virtual machines with the following settings (Note that this was done using VirtualBox, but this should work on VMWare as well):
    1. One will be named "pitcher" and the other will be named "catcher"
    2. They both should be using the Xubuntu ISO.
    3. Give them each a gigabyte or two of RAM.
    4. They should have two network adapters, one that's an internal network (for simulating an actual network), and another that has NAT (for accessing the internet).
    5. The network adapter using NAT is only going to be used to download the software we need from the internet, and it can be disabled later.
3. Run both virtual machines, install Xubuntu, create a user account, and update the OS.
4. Use `ip addr` to figure out the IP addresses. Then make sure that both machines can ping each other.
    - If they cannot ping each other, then you most likely need to look into your network adapter settings again.
5. Download wireshark on the pitcher and the catcher using `sudo apt install wireshark`.
    - Even though technically we don't need to use wireshark on the catcher, we might use the libraries that are installed with wireshark later.
6. Take a snapshot of both VMs.
7. On the pitcher, start capturing network traffic in wireshark for a minute or so, and then click `File->Export Packet Dissections->As PDML XML`. Save the file as "test.pdml" and close wireshark.
8. On the catcher, run `nc -nlvp 1234 > test.pdml` on the command line.
9. On the pitcher, run `nc [catcher's IP] 1234 < test.pdml` on the command line.
10. If you can open test.pdml on the catcher in a text editor and see XML data, then congratulations, you’ve just finished a basic one-way transfer.
