import sys
import pygame


class KeyStates():
    """Class that holds all states of keys."""
    def __init__(self):
        self.up = False
        self.down = False
        self.left = False
        self.right = False


def check_keydown_events(event, key_states):
    """Respond to key presses."""
    if event.key == pygame.K_RIGHT:
        key_states.right = True
        print("RIGHT Pressed")
    elif event.key == pygame.K_LEFT:
        key_states.left = True
        print("LEFT Pressed")
    elif event.key == pygame.K_UP:
        key_states.up = True
        print("UP Pressed")
    elif event.key == pygame.K_DOWN:
        key_states.down = True
        print("DOWN Pressed")



def check_keyup_events(event, key_states):
    """Respond to key releases."""
    if event.key == pygame.K_RIGHT:
        key_states.right = False
        print("RIGHT Released")
    elif event.key == pygame.K_LEFT:
        key_states.left = False
        print("LEFT Released")
    elif event.key == pygame.K_UP:
        key_states.up = False
        print("UP Released")
    elif event.key == pygame.K_DOWN:
        key_states.down = False
        print("DOWN Released")


def monitor_keyboard(key_states):
    """Respond to key presses """
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            sys.exit()
        elif event.type == pygame.KEYDOWN:
            check_keydown_events(event, key_states)
            return True
        elif event.type == pygame.KEYUP:
            check_keyup_events(event, key_states)
            return True

    # In case of no input we return false
    return False

