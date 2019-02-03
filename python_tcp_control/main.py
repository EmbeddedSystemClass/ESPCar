from sockettcp import *
from time import sleep
from keyboard import monitor_keyboard, KeyStates, create_message
import pygame

# -----------  Config  ----------
ip = '192.168.1.1'
port = 3000
# -------------------------------

# Initialise pygame and create new screen
pygame.init()
screen = pygame.display.set_mode((100, 100))
pygame.display.set_caption("RoboCar")

# Create object which will hold key states
key_states = KeyStates()

# Connect to server and expose sock object for further use
sock = socket_int(ip, port)
sock.setblocking(0)
print("Connected to " + ip + "\n")

while True:
    # Monitor keyboard
    commandWaiting = monitor_keyboard(key_states)

    # Send commands
    if commandWaiting:
        msg = create_message(key_states)
        send(sock, msg)
        # In case of button release wait 20 ms and send again
        if msg == "NONE":
            sleep(0.01)
            send(sock, msg)
            sleep(0.01)
            send(sock, msg)

    # Receive data
    receive(sock)
