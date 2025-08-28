; bootloader.asm
; Simple bootloader that prints "Hello, World!" and halts
; Must be exactly 512 bytes with 0x55AA signature at the end

[org 0x7C00]           ; Boot sector load address

start:
    ; Print the string
    mov si, message     ; SI points to the message string

print_loop:
    lodsb               ; Load byte at [SI] into AL, increment SI
    cmp al, 0           ; Check for null terminator
    je halt             ; If zero, end printing
    call print_char
    jmp print_loop

print_char:
    mov ah, 0x0E        ; BIOS teletype function
    mov bh, 0x00        ; Page number
    mov bl, 0x07        ; Text attribute (gray on black)
    int 0x10            ; BIOS video interrupt
    ret

halt:
    cli                 ; Disable interrupts
.hang:
    hlt                 ; Halt CPU
    jmp .hang

message db 'Hello Duniya, Chai Peelo !', 0

; Fill the rest of the sector with zeros
times 510 - ($ - $$) db 0

; Boot sector signature
dw 0xAA55
