// screen.c

#include "screen.h"

// VGA text mode starts at memory address 0xB8000
volatile unsigned short* vga_buffer = (unsigned short*)0xB8000;

int cursor_row = 0;
int cursor_col = 0;

unsigned char make_vga_entry(char c, char color) {
    return ((unsigned short)color << 8) | c;
}

void set_cursor(int row, int col) {
    cursor_row = row;
    cursor_col = col;
}

void print_char(char c, char color) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else {
        vga_buffer[cursor_row * VGA_WIDTH + cursor_col] = make_vga_entry(c, color);
        cursor_col++;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }

    // Handle screen overflow (basic scroll: reset to top)
    if (cursor_row >= VGA_HEIGHT) {
        clear_screen();
        cursor_row = 0;
        cursor_col = 0;
    }
}

void print_string(const char* str, char color) {
    for (int i = 0; str[i] != '\0'; i++) {
        print_char(str[i], color);
    }
}

void clear_screen() {
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[row * VGA_WIDTH + col] = make_vga_entry(' ', 0x07); // white on black
        }
    }

    set_cursor(0, 0);
}
