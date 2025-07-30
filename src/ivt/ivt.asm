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
global setup_ivt
setup_ivt:
    push ds
    xor ax, ax
    mov ds, ax
    xor si, si
.set_ivt_segments_loop:
    cmp si, 256
    je .set_ivt_addrs
    
    mov bx, si
    shl bx, 2
    mov word [bx], undefined_int
    mov word [bx + 2], 0xf000
    inc si
    jmp .set_ivt_segments_loop

.set_ivt_addrs:
    mov word [0x0 * 4], isr0
    ;mov word [0x1 * 4], isr1
    ;mov word [0x2 * 4], isr2
    ;mov word [0x3 * 4], isr3
    ;mov word [0x4 * 4], isr4
    ;mov word [0x5 * 4], isr5
    ;mov word [0x6 * 4], isr6
    ;mov word [0x7 * 4], isr7
    mov word [0x10 * 4], isr10
    mov word [0x8 * 4], irq0
    mov word [0x9 * 4], irq1
    mov word [0xa * 4], irq2
    mov word [0xb * 4], irq3
    mov word [0xc * 4], irq4
    mov word [0xd * 4], irq5
    mov word [0xe * 4], irq6
    mov word [0xf * 4], irq7
    mov word [0x70 * 4], irq8
    mov word [0x71 * 4], irq9
    mov word [0x72 * 4], irq10
    mov word [0x73 * 4], irq11
    mov word [0x74 * 4], irq12
    mov word [0x75 * 4], irq13
    mov word [0x76 * 4], irq14
    mov word [0x77 * 4], irq15
    
    mov al, 0x11
    out 0x20, al
    out 0xa0, al
    mov al, 0x8
    out 0x20, al
    mov al, 0x70
    out 0xa0, al
    mov al, 4
    out 0x21, al
    shr al, 1
    out 0xa1, al
    dec al
    out 0x21, al
    out 0xa1, al
    
    dec al
    out 0x21, al
    out 0xa1, al

    sti
    pop ds
    ret

pic_enable:
    push ax
    xor ax, ax
    out 0x20, al
    out 0xa0, al
    pop ax
    ret

pic_disable:
    push ax
    mov al, 0xff
    out 0x20, al
    out 0xa0, al
    pop ax
    ret

pic1_eoi:
    push ax
    mov al, 0x20
    out 0x20, al
    pop ax
    ret

pic2_eoi:
    push ax
    mov al, 0x20
    out 0xa0, al
    out 0x20, al
    pop ax
    ret

isr0: ; Divide by 0
    cli
    xor ax, ax
    xor dx, dx
    sti
    iret
isr1: ; Reserved
    iret
isr2: ; NMI
    iret
isr3: ; Breakpoint (Not implemented because the BIOS will not support debugging)
    iret
isr4: ; Overflow (Not implemented because the BIOS will not handle overflows for Operating Systems)
    iret
isr5: ; Bound range exceeded (Not implemented)
    iret
;isr6: ; Invalid Opcode (Will be implemented)
;    iret
isr7: ; Processor Extension Not Available (Not implemented)
    iret
isr10: ; A filler int 0x10 handler for Video Services
    cli

    cmp ah, 0xe ; Check if the Teletype function is needed
    je .teletype
    jmp .failure
.teletype:
    push dx
    mov dx, 0xe9
    out dx, al
    pop dx
    jmp .return

.failure:
    stc
.return:
    sti
    iret

irq0:
    cli
    call pic1_eoi
    sti
    iret
irq1:
    cli
    call pic1_eoi
    sti
    iret
irq2:
    cli
    call pic1_eoi
    sti
    iret
irq3:
    cli
    call pic1_eoi
    sti
    iret
irq4:
    cli
    call pic1_eoi
    sti
    iret
irq5:
    cli
    call pic1_eoi
    sti
    iret
irq6:
    cli
    call pic1_eoi
    sti
    iret
irq7:
    cli
    call pic1_eoi
    sti
    iret
irq8:
    cli
    call pic2_eoi
    sti
    iret
irq9:
    cli
    call pic2_eoi
    sti
    iret
irq10:
    cli
    call pic2_eoi
    sti
    iret
irq11:
    cli
    call pic2_eoi
    sti
    iret
irq12:
    cli
    call pic2_eoi
    sti
    iret
irq13:
    cli
    call pic2_eoi
    sti
    iret
irq14:
    cli
    call pic2_eoi
    sti
    iret
irq15:
    cli
    call pic2_eoi
    sti
    iret
undefined_int:
    iret
