bits 32
global serial_isr

extern serial_interrupt     ; void serial_interrupt(void);

serial_isr:
    ; Save general-purpose registers first
    pusha

    ; Save segment registers
    push ds
    push es
    push fs
    push gs

    ; Ensure we have the kernel data segment loaded
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call the C-level serial interrupt handler
    call serial_interrupt

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore general-purpose registers
    popa

    iret
    