#ifndef MOUSE_H
#define MOUSE_H

// PS/2 Mouse constants
#define MOUSE_PORT_DATA    0x60
#define MOUSE_PORT_STATUS  0x64
#define MOUSE_PORT_COMMAND 0x64

// Mouse status bits
#define MOUSE_STATUS_OUTPUT_FULL 0x01
#define MOUSE_STATUS_INPUT_FULL  0x02
#define MOUSE_STATUS_SYSTEM      0x04
#define MOUSE_STATUS_COMMAND     0x08
#define MOUSE_STATUS_TIMEOUT     0x40
#define MOUSE_STATUS_PARITY      0x80

// Mouse commands
#define MOUSE_CMD_ENABLE         0xA8
#define MOUSE_CMD_DISABLE        0xA7
#define MOUSE_CMD_READ_CONFIG    0x20
#define MOUSE_CMD_WRITE_CONFIG   0x60
#define MOUSE_CMD_SELF_TEST      0xAA
#define MOUSE_CMD_TEST_MOUSE     0xA9
#define MOUSE_CMD_RESET          0xFF

// Mouse packet structure
typedef struct {
    unsigned char left_button:1;
    unsigned char right_button:1;
    unsigned char middle_button:1;
    unsigned char always_1:1;
    unsigned char x_sign:1;
    unsigned char y_sign:1;
    unsigned char x_overflow:1;
    unsigned char y_overflow:1;
    char x_movement;
    char y_movement;
} mouse_packet_t;

// Mouse state
typedef struct {
    int x;
    int y;
    int left_button;
    int right_button;
    int middle_button;
    int visible;
} mouse_state_t;

// Mouse buffer
#define MOUSE_BUFFER_SIZE 64
typedef struct {
    mouse_packet_t packets[MOUSE_BUFFER_SIZE];
    int head;
    int tail;
    int count;
} mouse_buffer_t;

// Global mouse state
extern mouse_state_t mouse_state;
extern mouse_buffer_t mouse_buffer;

// Function declarations
void init_mouse();
void mouse_wait(unsigned char type);
void mouse_write(unsigned char data);
unsigned char mouse_read();
int mouse_available();
mouse_packet_t get_mouse_packet();
void process_mouse_packet(mouse_packet_t packet);
void update_mouse_cursor();
void show_mouse_cursor();
void hide_mouse_cursor();
void set_mouse_position(int x, int y);
void get_mouse_position(int* x, int* y);
int is_mouse_button_pressed(int button);
void poll_mouse();

#endif // MOUSE_H
