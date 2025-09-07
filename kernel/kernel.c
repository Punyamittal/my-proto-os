#include "kernel.h"
#include "screen.h"
#include "keyboard.h"
#include "mouse.h"
#include "network.h"
#include "json.h"
#include "langchain.h"
#include "shell.h"
#include "env.h"
#include "voice.h"
#include "assistant.h"

// VGA text mode buffer starts at 0xB8000
volatile unsigned short* vga_buffer = (volatile unsigned short*)0xB8000;

// VGA text mode screen dimensions are defined in screen.h

// Current cursor position
int cursor_row = 0;
int cursor_col = 0;

// Function to print a character with color attribute
void print_char(char c, char color) {
    unsigned short attribute = (unsigned short)color << 8;
    unsigned short* location = vga_buffer + (cursor_row * VGA_WIDTH + cursor_col);
    *location = c | attribute;
    cursor_col++;
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }
}

// Function to print a null-terminated string with color
void print_string(const char* str, char color) {
    while (*str != '\0') {
        print_char(*str, color);
        str++;
    }
}

// Initialize the kernel
void init_kernel() {
    // Initialize screen
    init_screen();
    
    // Initialize keyboard
    init_keyboard();
    
    // Initialize mouse
    init_mouse();
    
    // Initialize network (simulated)
    init_network();
    
    // Initialize environment
    init_environment();
    
    // Initialize voice system
    init_voice_system();
    
    // Initialize AI assistant system
    init_assistant_system();
    
    // Print welcome message
    print_string("ProtoOS Kernel with AI Assistant Integration!\n", VGA_LIGHT_GREEN);
    print_string("Welcome to your voice-controlled AI operating system!\n", VGA_LIGHT_CYAN);
    print_string("Default AI Model: Google Gemini\n", VGA_LIGHT_YELLOW);
    print_string("Voice Assistant: Always-on, ready for 'Hey Proto' commands\n", VGA_LIGHT_YELLOW);
    print_string("Mouse Support: PS/2 mouse driver initialized\n", VGA_LIGHT_BLUE);
    print_string("==========================================\n", VGA_LIGHT_GREY);
}

// Initialize system components
void init_system() {
    // Clear screen and set up display
    clear_screen();
    
    // Set up basic system state
    set_cursor(0, 0);
}

// Simple delay function
void delay(int milliseconds) {
    // This is a very basic delay - in a real OS you'd use a timer
    volatile int i;
    for (i = 0; i < milliseconds * 10000; i++) {
        // Busy wait
    }
}

// Panic function for critical errors
void panic(const char* message) {
    set_color(VGA_LIGHT_RED);
    print_string("\n*** KERNEL PANIC ***\n", VGA_LIGHT_RED);
    print_string(message, VGA_LIGHT_RED);
    print_string("\nSystem halted.\n", VGA_LIGHT_RED);
    
    // Halt the system
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

// Entry point for the kernel
void kernel_main() {
    // Initialize the system
    init_system();
    init_kernel();
    
    print_string("\nSystem ready. Starting AI Assistant with voice control...\n", VGA_LIGHT_GREY);
    delay(1000); // Give user time to read
    
    // Initialize and run the shell
    init_shell();
    run_shell();
    
    // If we ever return from the shell, halt the system
    print_string("\nShell exited. Halting system...\n", VGA_LIGHT_YELLOW);
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
