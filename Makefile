# Makefile for ProtoOS with AI Assistant Integration
# Builds a complete voice-controlled AI operating system

# Compiler and assembler
CC = gcc
AS = nasm
LD = ld

# Flags
CFLAGS = -m32 -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -fno-pic -mno-red-zone
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel
BUILD_DIR = build

# Source files
BOOT_SOURCES = $(BOOT_DIR)/bootloader.asm
KERNEL_SOURCES = $(KERNEL_DIR)/kernel.c $(KERNEL_DIR)/screen.c $(KERNEL_DIR)/keyboard.c \
                 $(KERNEL_DIR)/network.c $(KERNEL_DIR)/json.c $(KERNEL_DIR)/langchain.c \
                 $(KERNEL_DIR)/shell.c $(KERNEL_DIR)/env.c $(KERNEL_DIR)/voice.c \
                 $(KERNEL_DIR)/assistant.c

# Object files
BOOT_OBJECTS = $(BUILD_DIR)/bootloader.bin
KERNEL_OBJECTS = $(BUILD_DIR)/kernel.bin

# Final output
OS_IMAGE = $(BUILD_DIR)/protoos-ai-assistant.img

# Default target
all: $(OS_IMAGE)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build bootloader
$(BUILD_DIR)/bootloader.bin: $(BOOT_SOURCES) | $(BUILD_DIR)
	$(AS) -f bin -o $@ $<

# Build kernel
$(BUILD_DIR)/kernel.bin: $(KERNEL_SOURCES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/screen.c -o $(BUILD_DIR)/screen.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/keyboard.c -o $(BUILD_DIR)/keyboard.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/network.c -o $(BUILD_DIR)/network.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/json.c -o $(BUILD_DIR)/json.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/langchain.c -o $(BUILD_DIR)/langchain.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/shell.c -o $(BUILD_DIR)/shell.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/env.c -o $(BUILD_DIR)/env.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/voice.c -o $(BUILD_DIR)/voice.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/assistant.c -o $(BUILD_DIR)/assistant.o
	$(LD) $(LDFLAGS) -o $@ $(BUILD_DIR)/kernel.o $(BUILD_DIR)/screen.o $(BUILD_DIR)/keyboard.o \
		$(BUILD_DIR)/network.o $(BUILD_DIR)/json.o $(BUILD_DIR)/langchain.o $(BUILD_DIR)/shell.o \
		$(BUILD_DIR)/env.o $(BUILD_DIR)/voice.o $(BUILD_DIR)/assistant.o

# Create OS image
$(OS_IMAGE): $(BOOT_OBJECTS) $(KERNEL_OBJECTS)
	# Create a 1.44MB floppy disk image
	dd if=/dev/zero of=$@ bs=512 count=2880 2>/dev/null
	# Write bootloader to first sector
	dd if=$(BOOT_OBJECTS) of=$@ conv=notrunc bs=512 count=1 2>/dev/null
	# Write kernel starting from second sector
	dd if=$(KERNEL_OBJECTS) of=$@ conv=notrunc bs=512 seek=1 2>/dev/null

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Run in QEMU
run: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE) -m 16

# Run in Bochs (if available)
run-bochs: $(OS_IMAGE)
	bochs -f bochsrc.txt -q

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential nasm qemu-system-x86

# Install dependencies (macOS)
install-deps-mac:
	brew install nasm qemu

# Install dependencies (Windows with Chocolatey)
install-deps-win:
	choco install nasm qemu

# Help
help:
	@echo "ProtoOS with AI Assistant Integration - Build System"
	@echo "==================================================="
	@echo "make all          - Build complete voice-controlled AI OS"
	@echo "make clean        - Clean build files"
	@echo "make run          - Run in QEMU"
	@echo "make run-bochs    - Run in Bochs"
	@echo "make install-deps - Install dependencies (Ubuntu/Debian)"
	@echo "make install-deps-mac - Install dependencies (macOS)"
	@echo "make install-deps-win - Install dependencies (Windows)"

.PHONY: all clean run run-bochs install-deps install-deps-mac install-deps-win help
