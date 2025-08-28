// kernel.c

// VGA text mode buffer starts at 0xB8000
volatile unsigned short* vga_buffer = (unsigned short*)0xB8000;

// VGA text mode screen dimensions
const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;

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

// Function to print a null-terminated string
void print_string(const char* str) {
    while (*str != '\0') {
        print_char(*str, 0x07); // light gray on black
        str++;
    }
}

// Entry point for the kernel
void kernel_main() {
    print_string("Hello from kernel.c!\n");
    
    // Halt CPU
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
