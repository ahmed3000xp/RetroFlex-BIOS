# RetroFlex BIOS Project

## Overview
RetroFlex BIOS is a Free and Open Source Legacy MBR BIOS.

## Installation Instructions

### For Linux based Systems:

- **Debian/Ubuntu/Ubuntu-based:** `sudo apt install qemu-system-x86 nasm gcc make parted dosfstools libgmp-dev libmpc-dev libmpfr-dev texinfo-dev`

- **Fedora/Fedora-based:** `sudo dnf install nasm qemu @development-tools bison flex gmp-devel libmpc-devel mpfr-devel texinfo gcc make`

- **Arch/Arch-based:** `sudo pacman -Syy nasm bison flex gmp mpc mpfr texinfo gcc make qemu-full`

### Compiling The RetroFlex BIOS toolchain

Before you compile RetroFlex BIOS, you need to make sure you have the RetroFlex BIOS toolchain compiled by running:

`ls tools`

WARNING: The toolchain build script automatically uses all your CPU's cores and threads. You have been warned.
If you didn't find a directory named `RetroFlex-BIOS-cross` then you have to compile the RetroFlex BIOS toolchain by running these commands:

`cd tools/cross
./build-cross.sh`

After it finishes the toolchain should be compiled. To test it just compile RetroFlex BIOS by following the instructions in the Compiling and Running RetroFlex BIOS section.
If you encounter an error read it carefully and search it up or contact me.

### Compiling and Running RetroFlex BIOS

After installation, compile and run RetroFlex BIOS by executing:

`make all`

#### Running on x86/x86_64 Hardware

If you're on an x86 or x86_64 machine, you can run RetroFlex BIOS with KVM using:

`make qemu_kvm`

Note: KVM currently mishandles the 16-bit to 32-bit transitions in RetroFlex BIOS, causing freezes. Works perfectly on QEMU without KVM, PCem, 86Box, and real 386/486 hardware.

#### Issues

When you run QEMU with KVM the BIOS will freeze when switching to 32-bit Protected Mode and the BIOS freezes when far jumping into a 32-bit Protected Mode entry point. The BIOS currently works perfectly on PCem, 86Box, real 386/486 hardware and QEMU without KVM acceleration 

#### License

This project is licensed under the GNU GPL-3.0 license.

#### Contact

For support or questions, please contact me at [ahmed.retroflexos.dev@protonmail.com](mailto:ahmed.retroflexos.dev@protonmail.com)
