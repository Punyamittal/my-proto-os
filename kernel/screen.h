#ifndef SCREEN_H
#define SCREEN_H

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_BUFFER 0xB8000

// Color constants
#define VGA_BLACK 0x00
#define VGA_BLUE 0x01
#define VGA_GREEN 0x02
#define VGA_CYAN 0x03
#define VGA_RED 0x04
#define VGA_MAGENTA 0x05
#define VGA_BROWN 0x06
#define VGA_LIGHT_GREY 0x07
#define VGA_DARK_GREY 0x08
#define VGA_LIGHT_BLUE 0x09
#define VGA_LIGHT_GREEN 0x0A
#define VGA_LIGHT_CYAN 0x0B
#define VGA_LIGHT_RED 0x0C
#define VGA_LIGHT_MAGENTA 0x0D
#define VGA_LIGHT_BROWN 0x0E
#define VGA_LIGHT_YELLOW 0x0E  // Same as LIGHT_BROWN in VGA
#define VGA_WHITE 0x0F
#define VGA_LIGHT_WHITE 0x0F   // Same as WHITE
#define VGA_LIGHT_GREY 0x07    // Same as LIGHT_GREY

// Function declarations
void init_screen();
void clear_screen();
void set_cursor(int row, int col);
void print_char(char c, char color);
void print_string(const char* str, char color);
void print_string_at(const char* str, char color, int row, int col);
void scroll_screen();
void set_color(char color);
void print_char_at(char c, char color, int row, int col);
char get_char_at(int row, int col);
void save_char_under_mouse();
void restore_char_under_mouse();

#endif // SCREEN_H
