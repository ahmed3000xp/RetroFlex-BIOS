bits 16

section .bios

global dbg_bios_puts
global bios_start
global bootstrap_string
global main_bios_gdt
global main_bios_gdt_ptr
global main_bios_switch_32_bit ; After you call it you will need to enable Interrupts yourself. Before you call it you should set your 32-bit entry point address in the ROM in the EBX register. To call the function use the jmp instruction and not the call instruction
global main_bios_switch_16_bit ; MUST CALL THIS IN 32-BIT PROTECTED MODE. DON'T CALL IN 16-BIT REAL MODE. After you call it you will need to enable Interrupts yourself. Before you call it you should set your 16-bit real mode entry point address in the ROM in the BX register. To call the function use the jmp instruction and not the call instruction

extern setup_ivt
extern setup_sections
extern setup_pci
extern setup_vga
extern vga_bios_puts

bios_start:
    cli
    mov ax, 0x9000
    mov ss, ax
    mov es, ax
    mov ah, 0xf0
    mov ds, ax
    mov bp, ax
    mov sp, bp

    cld
    mov si, bootstrap_string
    call dbg_bios_puts

    mov ax, [0xfffe]
    cmp ax, 0xaa55
    je .continue_exec1
    jmp .print_invalid_sig

.continue_exec1:
    call setup_ivt
    call setup_vga
    call setup_sections
    call setup_pci
.halt:
    hlt
    jmp .halt
.print_invalid_sig:
    call dbg_bios_puts
    jmp .continue_exec1

dbg_bios_puts:
    mov dx, 0xe9
.loop:
    lodsb

    or al, al
    jz .return

    out dx, al

    jmp .loop
.return:
    ret

bootstrap_string: db "RetroFlex-BIOS", 0xd, 0xa, 0
invalid_bios_sig_string: db "Invalid BIOS 0xaa55 signature", 0xa, "The BIOS will continue BUT IT MIGHT BE CORRUPTED", 0xa, 0

main_bios_gdt:
    dd 0 ; Null Segment
    dd 0

; Code Segment 32-bit
    dw 0xffff ; Limit
    dw 0x0000 ; Base lower 16-bits
    db 0x00 ; Base middle 8-bits
    db 0b10011010 ; Access Byte. Present, Ring 0, Code/Data, Executable, Executable only to same Ring, Read Only, Accessed (Used by CPU but not needed for the BIOS)
    db 0b11001111 ; Lower 4-bits for upper 4-bits of Limit. Upper 4-bits for Flags. Granularity, 32-bit Segment
    db 0x00 ; Base upper 8-bits

; Data Segment 32-bit
    dw 0xffff ; Limit
    dw 0x0000 ; Base lower 16-bits
    db 0x00 ; Base middle 8-bits
    db 0b10010010 ; Access Byte. Present, Ring 0, Code/Data, Not Executable, Readable and Writeable only to same Ring, Read/Write, Accessed (Used by CPU but not needed for the BIOS)
    db 0b11001111 ; Lower 4-bits for upper 4-bits of Limit. Upper 4-bits for Flags. Granularity, 32-bit Segment
    db 0x00 ; Base upper 8-bits

; Code Segment 16-bit
    dw 0xffff ; Limit
    dw 0x0000 ; Base lower 16-bits
    db 0x0f ; Base middle 8-bits
    db 0b10011010 ; Access Byte. Present, Ring 0, Code/Data, Executable, Executable only to same Ring, Read Only, Accessed (Used by CPU but not needed for the BIOS)
    db 0b00000000 ; Lower 4-bits for upper 4-bits of Limit. Upper 4-bits for Flags. 16-bit Segment
    db 0x00 ; Base upper 8-bits

; Data Segment 16-bit
    dw 0xffff ; Limit
    dw 0x0000 ; Base lower 16-bits
    db 0x00 ; Base middle 8-bits
    db 0b10010010 ; Access Byte. Present, Ring 0, Code/Data, Not Executable, Readable and Writeable only to same Ring, Read/Write, Accessed (Used by CPU but not needed for the BIOS)
    db 0b00001111 ; Lower 4-bits for upper 4-bits of Limit. Upper 4-bits for Flags. 16-bit Segment
    db 0x00 ; Base upper 8-bits
main_bios_gdt_end:

main_bios_gdt_ptr:
    dw main_bios_gdt_end - main_bios_gdt ; Size of GDT
    dd main_bios_gdt + 0xf0000 ; Address of GDT

main_bios_idt_real_mode:
    dw 0x3ff
    dd 0

main_bios_switch_32_bit:
    cli

    lgdt [dword main_bios_gdt_ptr]
    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp 1*8:dword main_bios_protected_mode_entry

main_bios_16_bit_pm_entry:
    mov ax, 4*8
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, cr0
    and eax, 0xfffffffe
    mov cr0, eax

    jmp 0xf000:.real_mode_entry

.real_mode_entry:
    mov ax, 0
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ax, 0x9000
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ah, 0xf0
    mov ds, ax

    and ebp, 0xffff
    and esp, 0xffff

    lidt [main_bios_idt_real_mode]
    sti

    jmp bx

bits 32

section .bios32bit
main_bios_protected_mode_entry:
    mov ax, 2*8 ; 32-bit Data Segment number
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    or ebp, 0x90000
    or esp, 0x90000

    jmp ebx

main_bios_switch_16_bit:
    cli
    jmp 3*8:main_bios_16_bit_pm_entry

align 8
