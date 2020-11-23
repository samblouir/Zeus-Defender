
from config import *
import socket
import json

# default socket type
socket_connection_type = socket.AF_INET

def setSocketType(UDS_instead_of_IPV4):
    if UDS_instead_of_IPV4:
        socket_connection_type = socket.AF_UNIX
    else:
        socket_connection_type = socket.AF_INET
    
def send(target, msg):
    """
    :param target: The destination (e.g.: put ReceiverAddr if you are the Pitcher)
    :param msg: An array of any object-type. Element 0 MUST be the sender's name.
        send(FlagAddr, [DataAnalyticsName, *received_payload])
    """
    with socket.socket(socket_connection_type, socket.SOCK_STREAM) as s:
        s.connect(target)
        msg = json.dumps(msg)
        s.sendall(msg.encode())
        s.close()

def listen(AddrToListenAt):
    """
    :param AddrToListenAt: Pass in a tuple of the IP Address and Port to listen at
    :return: Returns the String of the Source's name, and a
    """
    with socket.socket(socket_connection_type, socket.SOCK_STREAM) as s:
        s.bind(AddrToListenAt)
        s.listen()
        conn, addr = s.accept()
        with conn:
            received_data = conn.recv(1024)
            data = json.loads(received_data)
            s.close()
            return data[0], data[1:]

def send_to_Filter(msg):
    """

    :param msg:
    :return:
    """
    return send(FilterAddr, msg)


def send_to_Receiver(msg):
    return send(ReceiverAddr, msg)


def send_to_DataAnalytics(msg):
    return send(DataAnalyticsAddr, msg)


def send_to_Flag(msg):
    return send(FlagAddr, msg)
