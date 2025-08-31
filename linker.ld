/* linker.ld - Linker script for ProtoOS kernel */

OUTPUT_FORMAT(binary)
ENTRY(kernel_main)

SECTIONS
{
    /* Set the load address of the kernel to 0x1000 (4KB) */
    . = 0x1000;

    /* Code section */
    .text : {
        *(.text)
        *(.rodata)
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
