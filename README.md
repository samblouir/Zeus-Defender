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
7. On the catcher, run `nc -nlvU ZD` to create a UDS socket that acts as the filter.
8. On the catcher, run `./receiver <Pitcher's IP address> <PORT>` to start the receiver.
9. On the pitcher, run `./pitcher <XML file> <Reciever's IP address> <Receiver's PORT>` to send the XML file.
10. The catcher should be able to see the XML file sent from the pitcher and forward it to the netcat server that is acting as the filter.
