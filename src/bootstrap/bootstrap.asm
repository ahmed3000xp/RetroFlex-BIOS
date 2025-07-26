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

section .bootstrap_cpu
extern bios_start
global bootstrap_cpu
bootstrap_cpu:
    jmp 0xf000:0

section .bios_signature
bios_signature:
    db 0x55
    db 0xaa
