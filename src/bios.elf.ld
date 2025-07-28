ENTRY(bootstrap_cpu)

SECTIONS
{
    . = 0xfff0;
    .bootstrap_cpu 0xfff0 : {
        *(.bootstrap_cpu)
    }

    . = 0xfffe;
    .bios_signature : {
        *(.bios_signature)
    }

    . = 0x0000;

    .bios : {
        *(.bios)
    }
    _rom_file_offset = SIZEOF(.bios);
    _rom_vma_current_addr = _rom_file_offset;

    .bios32bit _rom_file_offset + 0xf0000 : AT(_rom_file_offset) {
        *(.bios32bit)
    }
    _rom_file_offset = _rom_file_offset + SIZEOF(.bios32bit);
    _rom_vma_current_addr = _rom_file_offset + 0xf0000;

    .text _rom_vma_current_addr : AT(_rom_file_offset) {
        *(.text)
    }
    _rom_file_offset = _rom_file_offset + SIZEOF(.text);
    _rom_vma_current_addr = _rom_file_offset + 0xf0000;

    .rodata _rom_vma_current_addr : AT(_rom_file_offset) {
        _rodata_start = .;
        *(.rodata*)
        _rodata_end = .;
    }
    _rom_file_offset += SIZEOF(.rodata);
    _rom_vma_current_addr = _rom_file_offset + 0xf0000;

    .data 0x8000a : AT(_rom_file_offset) {
        _data_start = .;
        *(.data)
        _data_end = .;
    }
    _data_load_start = LOADADDR(.data);
    _rom_file_offset += SIZEOF(.data);
    _rom_vma_current_addr = _rom_file_offset + 0x80000;

    .bss _rom_vma_current_addr : AT(_rom_file_offset) {
        _bss_start = .;
        *(.bss)
        _bss_end = .;
    }
    _rom_file_offset += SIZEOF(.bss);
    _rom_vma_current_addr = _rom_file_offset;

    end = .;
}
