bits 16

section .bios
global setup_i440fx_dram

extern dbg_bios_puts
setup_i440fx_dram:
    mov si, i440fx_dram_setup_unimplemented_string
    call dbg_bios_puts
    ret

i440fx_dram_setup_unimplemented_string: db "i440FX chipset DRAM init is still not implemented", 0xa, 0
