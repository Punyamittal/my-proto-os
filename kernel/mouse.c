#include "mouse.h"
#include "screen.h"

// Global mouse state
mouse_state_t mouse_state = {0, 0, 0, 0, 0, 1};
mouse_buffer_t mouse_buffer = {{0}, 0, 0, 0};

// Wait for mouse controller to be ready
void mouse_wait(unsigned char type) {
    unsigned int timeout = 100000;
    
    if (type == 0) {
        // Wait for input buffer to be empty
        while (timeout--) {
            if ((inb(MOUSE_PORT_STATUS) & MOUSE_STATUS_INPUT_FULL) == 0) {
                return;
            }
        }
    } else {
        // Wait for output buffer to be full
        while (timeout--) {
            if ((inb(MOUSE_PORT_STATUS) & MOUSE_STATUS_OUTPUT_FULL) == 1) {
                return;
            }
        }
    }
}

// Write data to mouse
void mouse_write(unsigned char data) {
    mouse_wait(0); // Wait for input buffer to be empty
    outb(MOUSE_PORT_COMMAND, 0xD4); // Tell controller we're sending mouse data
    mouse_wait(0);
    outb(MOUSE_PORT_DATA, data); // Send the data
}

// Read data from mouse
unsigned char mouse_read() {
    mouse_wait(1); // Wait for output buffer to be full
    return inb(MOUSE_PORT_DATA);
}

// Initialize the mouse
void init_mouse() {
    unsigned char status;
    
    // Clear mouse state
    mouse_state.x = VGA_WIDTH / 2;
    mouse_state.y = VGA_HEIGHT / 2;
    mouse_state.left_button = 0;
    mouse_state.right_button = 0;
    mouse_state.middle_button = 0;
    mouse_state.visible = 1;
    
    // Clear mouse buffer
    mouse_buffer.head = 0;
    mouse_buffer.tail = 0;
    mouse_buffer.count = 0;
    
    // Enable auxiliary mouse device
    mouse_wait(0);
    outb(MOUSE_PORT_COMMAND, MOUSE_CMD_ENABLE);
    
    // Enable mouse interrupts
    mouse_wait(0);
    outb(MOUSE_PORT_COMMAND, MOUSE_CMD_READ_CONFIG);
    mouse_wait(1);
    status = inb(MOUSE_PORT_DATA);
    status |= 0x02; // Enable mouse interrupt
    status |= 0x01; // Enable keyboard interrupt
    
    mouse_wait(0);
    outb(MOUSE_PORT_COMMAND, MOUSE_CMD_WRITE_CONFIG);
    mouse_wait(0);
    outb(MOUSE_PORT_DATA, status);
    
    // Set mouse to use default settings
    mouse_write(0xF6);
    mouse_read(); // Acknowledge
    
    // Enable the mouse
    mouse_write(0xF4);
    mouse_read(); // Acknowledge
    
    // Set mouse to stream mode
    mouse_write(0xEA);
    mouse_read(); // Acknowledge
}

// Check if mouse data is available
int mouse_available() {
    return mouse_buffer.count > 0;
}

// Get a mouse packet from the buffer
mouse_packet_t get_mouse_packet() {
    mouse_packet_t packet = {0};
    
    if (mouse_buffer.count > 0) {
        packet = mouse_buffer.packets[mouse_buffer.head];
        mouse_buffer.head = (mouse_buffer.head + 1) % MOUSE_BUFFER_SIZE;
        mouse_buffer.count--;
    }
    
    return packet;
}

// Add a mouse packet to the buffer
void add_mouse_packet(mouse_packet_t packet) {
    if (mouse_buffer.count < MOUSE_BUFFER_SIZE) {
        mouse_buffer.packets[mouse_buffer.tail] = packet;
        mouse_buffer.tail = (mouse_buffer.tail + 1) % MOUSE_BUFFER_SIZE;
        mouse_buffer.count++;
    }
}

// Process a mouse packet and update mouse state
void process_mouse_packet(mouse_packet_t packet) {
    // Update button states
    mouse_state.left_button = packet.left_button;
    mouse_state.right_button = packet.right_button;
    mouse_state.middle_button = packet.middle_button;
    
    // Update position
    int new_x = mouse_state.x + packet.x_movement;
    int new_y = mouse_state.y - packet.y_movement; // Invert Y for screen coordinates
    
    // Clamp to screen boundaries
    if (new_x >= 0 && new_x < VGA_WIDTH) {
        mouse_state.x = new_x;
    }
    if (new_y >= 0 && new_y < VGA_HEIGHT) {
        mouse_state.y = new_y;
    }
    
    // Update cursor display
    update_mouse_cursor();
}

// Update mouse cursor display
void update_mouse_cursor() {
    if (mouse_state.visible) {
        // In a real implementation, you'd save/restore the character under the cursor
        // For now, we'll just show a simple cursor character
        print_string_at(">", VGA_LIGHT_CYAN, mouse_state.y, mouse_state.x);
    }
}

// Show mouse cursor
void show_mouse_cursor() {
    mouse_state.visible = 1;
    update_mouse_cursor();
}

// Hide mouse cursor
void hide_mouse_cursor() {
    mouse_state.visible = 0;
    // In a real implementation, you'd restore the character under the cursor
}

// Set mouse position
void set_mouse_position(int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        mouse_state.x = x;
        mouse_state.y = y;
        update_mouse_cursor();
    }
}

// Get mouse position
void get_mouse_position(int* x, int* y) {
    *x = mouse_state.x;
    *y = mouse_state.y;
}

// Check if a mouse button is pressed
int is_mouse_button_pressed(int button) {
    switch (button) {
        case 0: return mouse_state.left_button;
        case 1: return mouse_state.right_button;
        case 2: return mouse_state.middle_button;
        default: return 0;
    }
}

// Poll mouse for input (simplified version)
void poll_mouse() {
    // Check if mouse data is available
    if ((inb(MOUSE_PORT_STATUS) & MOUSE_STATUS_OUTPUT_FULL) == 1) {
        unsigned char data = inb(MOUSE_PORT_DATA);
        
        // This is a simplified polling implementation
        // In a real OS, you'd use interrupts and proper packet parsing
        static int packet_byte = 0;
        static mouse_packet_t current_packet = {0};
        
        switch (packet_byte) {
            case 0:
                // First byte - status byte
                current_packet.left_button = (data & 0x01) ? 1 : 0;
                current_packet.right_button = (data & 0x02) ? 1 : 0;
                current_packet.middle_button = (data & 0x04) ? 1 : 0;
                current_packet.x_sign = (data & 0x10) ? 1 : 0;
                current_packet.y_sign = (data & 0x20) ? 1 : 0;
                current_packet.x_overflow = (data & 0x40) ? 1 : 0;
                current_packet.y_overflow = (data & 0x80) ? 1 : 0;
                packet_byte = 1;
                break;
            case 1:
                // Second byte - X movement
                current_packet.x_movement = (char)data;
                packet_byte = 2;
                break;
            case 2:
                // Third byte - Y movement
                current_packet.y_movement = (char)data;
                process_mouse_packet(current_packet);
                packet_byte = 0;
                break;
        }
    }
}

// Assembly functions for port I/O
static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

static inline void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__("outb %0, %1" : : "a" (data), "Nd" (port));
}
