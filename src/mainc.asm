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

extern main_bios_switch_32_bit
extern main_bios_switch_16_bit

extern setup_sections_c

global setup_sections

setup_sections:
    mov ebx, setup_sections_protected_mode_entry
    call main_bios_switch_32_bit
setup_sections_return_to_caller:
    ret

bits 32
section .bios32bit
setup_sections_protected_mode_entry:
    call setup_sections_c
.setup_bda_and_ebda:
    mov word [0x40e], 0x8000
    mov word [0x413], 0x1
    mov word [0x449], 3
    mov word [0x44a], 80

    mov byte [0x80000], 'R'
    mov byte [0x80001], 'F'
    mov byte [0x80002], 'X'
    mov byte [0x80003], '0'
    mov word [0x80004], 1
.return:
    mov bx, setup_sections_return_to_caller
    jmp main_bios_switch_16_bit
