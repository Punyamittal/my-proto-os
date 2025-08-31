; bootloader.asm
; Bootloader that loads the kernel from disk and jumps to it
; Must be exactly 512 bytes with 0x55AA signature at the end

[org 0x7C00]           ; Boot sector load address
[bits 16]              ; 16-bit real mode

; Constants
KERNEL_OFFSET equ 0x1000    ; Kernel load address
KERNEL_SECTORS equ 20       ; Number of sectors to read (adjust as needed)

start:
    ; Initialize segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Print loading message
    mov si, loading_msg
    call print_string

    ; Load kernel from disk
    mov bx, KERNEL_OFFSET   ; Load to this address
    mov ah, 0x02            ; Read sectors function
    mov al, KERNEL_SECTORS  ; Number of sectors to read
    mov ch, 0x00            ; Cylinder 0
    mov cl, 0x02            ; Sector 2 (sector 1 is bootloader)
    mov dh, 0x00            ; Head 0
    mov dl, 0x00            ; Drive 0 (floppy)
    int 0x13                ; BIOS disk interrupt

    ; Check for errors
    jc disk_error

    ; Print success message
    mov si, success_msg
    call print_string

    ; Switch to protected mode
    cli                     ; Disable interrupts
    lgdt [gdt_descriptor]   ; Load GDT

    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Set PE bit in CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Jump to 32-bit code
    jmp 0x08:protected_mode

; 16-bit functions
print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

disk_error:
    mov si, error_msg
    call print_string
    jmp $

; 32-bit protected mode code
[bits 32]
protected_mode:
    ; Set up segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up stack
    mov esp, 0x90000

    ; Jump to kernel
    call KERNEL_OFFSET

    ; If we return, halt
    cli
.hang:
    hlt
    jmp .hang

; GDT
gdt_start:
    dq 0x0000000000000000  ; Null descriptor
    dq 0x00CF9A000000FFFF  ; Code descriptor (0x08)
    dq 0x00CF92000000FFFF  ; Data descriptor (0x10)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Messages
loading_msg db 'Loading ProtoOS...', 0x0D, 0x0A, 0
success_msg db 'Kernel loaded successfully!', 0x0D, 0x0A, 0
error_msg db 'Disk read error!', 0x0D, 0x0A, 0

; Fill the rest of the sector with zeros
times 510 - ($ - $$) db 0

; Boot sector signature
dw 0xAA55
