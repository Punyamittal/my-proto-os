#ifndef KERNEL_H
#define KERNEL_H

// Main kernel entry point
void kernel_main();

// Kernel initialization functions
void init_kernel();
void init_system();

// Utility functions
void delay(int milliseconds);
void panic(const char* message);

#endif // KERNEL_H
