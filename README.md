# Zeus-Defender

### Secondary git (to merge later)
https://github.com/samblouir/zeusids_installer

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
