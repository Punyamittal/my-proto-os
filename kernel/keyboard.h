// keyboard.h

#ifndef KEYBOARD_H
#define KEYBOARD_H

// Initializes the keyboard (usually sets up interrupt handler)
void init_keyboard();

// Reads a single character (blocking or from buffer, depending on implementation)
char get_char();

#endif
