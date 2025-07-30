; Copyright (C) 2025 Ahmed
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <https://www.gnu.org/licenses/>.

bits 16

section .bios
extern bootstrap_string
extern dbg_bios_puts
global vga_bios_puts
global setup_vga
setup_vga:
    pop dx
    mov [ss:0x6000], dx
    mov ax, 0xc000
    mov ds, ax

    cmp word [0], 0xaa55
    jne .invalid_rom

    db 0x9a, 0x03, 0x00, 0x00, 0xc0 ; call far 0xc000:0x0003 in machine code because NASM gave me operand errors if I assembled 'call far 0xc000:0x0003' normally so I put machine code instead
    mov ax, 0xf000
    mov ds, ax
    
    xor ah, ah
    mov al, 3
    int 0x10
    
    mov si, bootstrap_string
    call vga_bios_puts
    jmp .return

.invalid_rom:
    mov ax, 0xf000
    mov ds, ax
    mov si, invalid_vga_rom_sig_string
    call vga_bios_puts

.return:
    mov ax, 0x9000
    mov ss, ax
    mov es, ax
    mov ah, 0xf0
    mov ds, ax
    mov sp, 0xfffe
    mov bp, sp

    mov dx, [ss:0x6000]
    push dx
    ret

vga_bios_puts:
.loop:
    lodsb

    or al, al
    jz .return

    mov ah, 0x0e
    int 0x10

    jmp .loop
.return:
    ret

invalid_vga_rom_sig_string: db "Invalid VGA Option ROM Signature The BIOS will Continue execution with fake int 0x10 (Video) services", 0xa, 0
