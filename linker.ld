/* linker.ld - Simple linker script for kernel */

OUTPUT_FORMAT(elf32-i386)
ENTRY(kernel_main)

SECTIONS
{
    /* Set the load address of the kernel to 0x100000 (1MB) */
    . = 0x00100000;

    /* Code section */
    .text : {
        *(.multiboot)         /* Multiboot header if present */
        *(.text)              /* All .text sections */
        *(.rodata)            /* Read-only data */
    }

    /* Initialized data */
    .data : {
        *(.data)
    }

    /* Uninitialized data */
    .bss : {
        *(.bss)
        *(COMMON)
    }

    /* Align to 4KB boundary */
    . = ALIGN(0x1000);
}
