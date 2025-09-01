#include "screen.h"
#include "mouse.h"

// VGA text mode buffer
volatile unsigned short* vga_buffer = (unsigned short*)VGA_BUFFER;

// Current cursor position
int cursor_row = 0;
int cursor_col = 0;

// Current color attribute
char current_color = VGA_LIGHT_GREY;

// Create a VGA entry (character + color attribute)
unsigned short make_vga_entry(char c, char color) {
    return (unsigned short)c | (unsigned short)(color << 8);
}

// Initialize the screen
void init_screen() {
    clear_screen();
    set_cursor(0, 0);
}

// Clear the entire screen
void clear_screen() {
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[row * VGA_WIDTH + col] = make_vga_entry(' ', current_color);
        }
    }
    set_cursor(0, 0);
}

// Set cursor position
void set_cursor(int row, int col) {
    if (row >= 0 && row < VGA_HEIGHT && col >= 0 && col < VGA_WIDTH) {
        cursor_row = row;
        cursor_col = col;
    }
}

// Print a single character
void print_char(char c, char color) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else if (c == '\r') {
        cursor_col = 0;
    } else if (c == '\t') {
        cursor_col = (cursor_col + 4) & ~3; // Align to 4-character boundary
    } else {
        vga_buffer[cursor_row * VGA_WIDTH + cursor_col] = make_vga_entry(c, color);
        cursor_col++;
    }

    // Handle line wrapping
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }

    // Handle screen overflow
    if (cursor_row >= VGA_HEIGHT) {
        scroll_screen();
    }
}

// Print a string with specified color
void print_string(const char* str, char color) {
    for (int i = 0; str[i] != '\0'; i++) {
        print_char(str[i], color);
    }
}

// Print a string at specific position
void print_string_at(const char* str, char color, int row, int col) {
    int old_row = cursor_row;
    int old_col = cursor_col;
    
    set_cursor(row, col);
    print_string(str, color);
    
    set_cursor(old_row, old_col);
}

// Scroll the screen up by one line
void scroll_screen() {
    // Move all lines up by one
    for (int row = 1; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[(row - 1) * VGA_WIDTH + col] = vga_buffer[row * VGA_WIDTH + col];
        }
    }
    
    // Clear the bottom line
    for (int col = 0; col < VGA_WIDTH; col++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + col] = make_vga_entry(' ', current_color);
    }
    
    cursor_row = VGA_HEIGHT - 1;
    cursor_col = 0;
}

// Set the current color
void set_color(char color) {
    current_color = color;
}

// Print a character at specific position (for mouse cursor)
void print_char_at(char c, char color, int row, int col) {
    if (row >= 0 && row < VGA_HEIGHT && col >= 0 && col < VGA_WIDTH) {
        vga_buffer[row * VGA_WIDTH + col] = make_vga_entry(c, color);
    }
}

// Get character at specific position
char get_char_at(int row, int col) {
    if (row >= 0 && row < VGA_HEIGHT && col >= 0 && col < VGA_WIDTH) {
        return (char)(vga_buffer[row * VGA_WIDTH + col] & 0xFF);
    }
    return ' ';
}

// Save character under mouse cursor
char saved_char_under_mouse = ' ';
int saved_mouse_x = 0;
int saved_mouse_y = 0;

// Save character under mouse cursor position
void save_char_under_mouse() {
    if (mouse_state.visible) {
        saved_char_under_mouse = get_char_at(mouse_state.y, mouse_state.x);
        saved_mouse_x = mouse_state.x;
        saved_mouse_y = mouse_state.y;
    }
}

// Restore character under mouse cursor
void restore_char_under_mouse() {
    if (saved_mouse_x >= 0 && saved_mouse_x < VGA_WIDTH && 
        saved_mouse_y >= 0 && saved_mouse_y < VGA_HEIGHT) {
        print_char_at(saved_char_under_mouse, current_color, saved_mouse_y, saved_mouse_x);
    }
}
