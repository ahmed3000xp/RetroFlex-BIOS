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
extern dbg_bios_puts

extern pci_init
extern pci_list_devices
extern puts

global setup_pci

setup_pci:
    mov ebx, setup_pci_protected_mode_entry
    jmp main_bios_switch_32_bit
setup_pci_return_to_caller:
    ret

bits 32

section .bios32bit
setup_pci_protected_mode_entry:
    call pci_init
    call pci_list_devices
    mov bx, setup_pci_return_to_caller
    jmp main_bios_switch_16_bit
