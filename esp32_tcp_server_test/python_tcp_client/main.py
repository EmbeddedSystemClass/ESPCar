from sockettcp import *
import sys
from keyboard import monitor_keyboard, KeyStates
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
print("Connected to " + ip + "\n")

while True:
    # Monitor keyboard
    commandWaiting = monitor_keyboard(key_states)

    # Send commands
    if commandWaiting:
        send(sock, key_states)