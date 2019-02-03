import socket
import sys


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


def send(sock, msg):
    """Send message to ESP"""
    # default encoding to utf-8
    msg = msg.encode()
    sock.sendall(msg)

    #sock.close()


def receive(sock):
    """Receive message, in case of blocking error fail silently"""
    try:
        data = sock.recv(22)
    except BlockingIOError:
        # In case of
        pass
    else:
        if data:
            data = str(data.decode('unicode_escape'))
            print(data)


