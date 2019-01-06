import socket
import sys
from keyboard import KeyStates


def socket_int(ip, port):
    """Create socket and connect to TCP server"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as msg:
        print('Could not create socket: ' + str(msg[0]) + ': ' + msg[1])
        sys.exit(1)

    try:
        sock.connect((ip, port))
    except socket.error as msg:
        print('Could not open socket: ', msg)
        sock.close()
        sys.exit(1)

    return sock


def send(sock, key_states):

    if key_states.up or key_states.down or key_states.left or key_states.right:
        msg = "1"
    else:
        msg = "0"



    # default encoding to utf-8
    msg = msg.encode()
    sock.sendall(msg)

    #data = sock.recv(1024)

    #if data:
    #    print('Reply: ' + data.decode())


    #sock.close()

