// keyboard.h

#ifndef KEYBOARD_H
#define KEYBOARD_H

// Initialize the keyboard
void init_keyboard();

// Check if a key is available
int key_available();

// Read a single character (blocking or from buffer, depending on implementation)
char get_char();

// Add a key to the buffer (for testing/demo purposes)
void add_key_to_buffer(char key);

// Poll keyboard for input (for demo purposes)
void poll_keyboard();

#endif // KEYBOARD_H
