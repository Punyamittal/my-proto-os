#include "keyboard.h"
#include "screen.h"

// Keyboard buffer
#define KEYBOARD_BUFFER_SIZE 256
char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
int buffer_head = 0;
int buffer_tail = 0;
int buffer_count = 0;

// PS/2 keyboard scancode to ASCII conversion table
static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Initialize keyboard
void init_keyboard() {
    // Clear buffer
    buffer_head = 0;
    buffer_tail = 0;
    buffer_count = 0;
    
    // For now, we'll use polling instead of interrupts
    // In a real OS, you'd set up an interrupt handler
}

// Check if a key is available
int key_available() {
    return buffer_count > 0;
}

// Read a character from keyboard buffer
char get_char() {
    if (buffer_count == 0) {
        return 0; // No key available
    }
    
    char key = keyboard_buffer[buffer_head];
    buffer_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
    buffer_count--;
    
    return key;
}

// Add a character to the keyboard buffer
void add_key_to_buffer(char key) {
    if (buffer_count < KEYBOARD_BUFFER_SIZE) {
        keyboard_buffer[buffer_tail] = key;
        buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
        buffer_count++;
    }
}

// Simple keyboard polling function (for demonstration)
void poll_keyboard() {
    // This is a simplified version - in reality you'd read from port 0x60
    // For now, we'll just simulate some input
    static int demo_counter = 0;
    
    if (demo_counter++ > 1000) { // Simulate key press every 1000 calls
        demo_counter = 0;
        add_key_to_buffer('A' + (demo_counter % 26));
    }
}
