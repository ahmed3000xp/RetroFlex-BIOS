/*
 * Copyright (C) 2025 Ahmed
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "../headers/stdint.h"
#include "../headers/stdio.h"
#include "../headers/util.h"

#define PCI_MAX_DEVICES 40

#define PCI_CONFIG_ADDRESS_PORT 0xcf8
#define PCI_CONFIG_DATA_PORT 0xcfc

#define PCI_BAR_ADDRESSING_TYPE_MEMORY 0
#define PCI_BAR_ADDRESSING_TYPE_IO 1

#define PCI_MEMORY_BAR_TYPE_16_BIT 1
#define PCI_MEMORY_BAR_TYPE_32_BIT 0
#define PCI_MEMORY_BAR_TYPE_64_BIT 2

#define PCI_POST_CODE_MAPPING_32_BIT_BAR 0xcfc0
#define PCI_POST_CODE_MAPPING_64_BIT_BAR 0xcfc1
#define PCI_POST_CODE_MAPPING_IO_BAR 0xcfc2
#define PCI_POST_CODE_MAPPING_ROM_BAR 0xcfc3
#define PCI_POST_CODE_UNSUPPORTED_BAR 0xcfc4
#define PCI_POST_CODE_COPYING_OPTION_ROM_TO_RAM 0xcfc5
#define PCI_POST_CODE_SCANNING_DEVICES 0xcfcd
#define PCI_POST_CODE_DETECTED_DEVICE 0xcfcc
#define PCI_POST_CODE_DETECTED_CONTROLLER 0xcfce
#define PCI_POST_CODE_DIDNT_DETECT_CONTROLLER 0xcfcf

#define PCI_DEVICE_CLASS_CODE_DISPLAY_VGA_CONTROLLER 0x030000
#define PCI_DEVICE_CLASS_CODE_DISPLAY_8514_COMPATIBLE 0x030001
#define PCI_DEVICE_CLASS_CODE_DISPLAY_XGA_CONTROLLER 0x030100
#define PCI_DEVICE_CLASS_CODE_DISPLAY_3D_CONTROLLER 0x030200 // The PCI spec says that 3D controllers are not always VGA-Compatible, but with my testing, most Gaming and basic GPUs (Graphics Processing Units) are VGA-Compatible, even ones with 3D Controller class codes
#define PCI_DEVICE_CLASS_CODE_DISPLAY_OTHER 0x038000

#define PCI_DEVICE_CLASS_CODE_MASK 0x00ff0000
#define PCI_DEVICE_SUBCLASS_CODE_MASK 0x0000ff00
#define PCI_DEVICE_PROG_IF_CODE_MASK 0x000000ff

struct __attribute__((packed)) pci_header_type_0 {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
    uint32_t cardbus_cis_pointer;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_device_id;
    uint32_t expansion_rom_base_address;
    uint16_t capabilities_pointer;
    uint32_t reserved1 : 24;
    uint32_t reserved2;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
};

struct __attribute__((packed)) pci_header_type_1 {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t bar0;
    uint32_t bar1;
    uint8_t primary_bus_number;
    uint8_t secondary_bus_number;
    uint8_t subordinate_bus_number;
    uint8_t secondary_latency_timer;
    uint8_t io_base;
    uint8_t io_limit;
    uint16_t secondary_status;
    uint16_t memory_base;
    uint16_t memory_limit;
    uint16_t prefetchable_memory_base;
    uint16_t prefetchable_memory_limit;
    uint32_t prefetchable_base_upper_32_bits;
    uint32_t prefetchable_limit_upper_32_bits;
    uint16_t io_base_upper_16_bits;
    uint16_t io_limit_upper_16_bits;
    uint8_t capability_pointer;
    uint32_t reserved : 24;
    uint32_t expansion_rom_base_address;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint32_t bridge_control : 24;
};

struct __attribute__((packed)) pci_header_type_2 {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint32_t cardbus_socket_base_address;
    uint8_t capabilities_list_offset;
    uint8_t reserved1;
    uint16_t secondary_status;
    uint8_t pci_bus_number;
    uint8_t cardbus_bus_number;
    uint8_t subordinate_bus_number;
    uint8_t cardbus_latency_timer;
    uint32_t memory_base_address0;
    uint32_t memory_limit0;
    uint32_t memory_base_address1;
    uint32_t memory_limit1;
    uint32_t io_base_address0;
    uint32_t io_limit0;
    uint32_t io_base_address1;
    uint32_t io_limit1;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint16_t bridge_control;
    uint16_t subsystem_device_id;
    uint16_t subsystem_vendor_id;
    uint32_t pc_card_legacy_base_address;
};

struct __attribute__((packed)) pci_memory_bar {
    bool addressing_type : 1;
    uint8_t bar_type : 2;
    bool prefetchable : 1;
    uint32_t bar_address : 28;
};

struct __attribute__((packed)) pci_io_bar {
    bool addressing_type : 1;
    bool reserved : 1;
    uint32_t bar_address : 30;
};

struct __attribute__((packed)) pci_rom_bar {
    bool enable_bit : 1;
    uint16_t reserved : 10;
    uint32_t bar_address : 21;
};

struct __attribute__((packed)) pci_device_info {
    uint8_t device_bus;
    uint8_t device_slot;
    uint8_t device_func;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t device_type;
    bool device_unknown : 1;
    bool device_nvidia : 1; // This will be only enabled for NVIDIA devices!!!!!!
    uint32_t *device_header_data; // This should be typecasted based on the device_type variable, unless you are handling it as an uint32_t pointer intentionally
    uint8_t interrupt_pin : 3; // This is the PCI Interrupt PIN like for example INTA#, INTB#, INTC#, INTD# and this is set up by the BIOS for every PCI device
    uint8_t interrupt_line : 4; // This is the PIC Interrupt Line number that the PCI device uses
    void (*device_interrupt_handler)(); // There are no drivers for any PCI device in the time being so this will always be NULL 
};

extern uint32_t pci_devices_headers[PCI_MAX_DEVICES][sizeof(struct pci_header_type_2)];
extern struct pci_device_info pci_devices[PCI_MAX_DEVICES];
extern struct pci_device_info *pci_devices_irqs[16][5];

bool pci_init();
uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg_offset);
void pci_config_write(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg_offset, uint32_t data);
void pci_read_header(uint8_t bus, uint8_t device, uint8_t func, struct pci_device_info *pci_device);
void pci_setup_device(uint8_t bus, uint8_t device, uint8_t func, struct pci_device_info *pci_device);
void pci_init_device(uint8_t bus, uint8_t device, uint8_t func); // This function supports only specific Prog IFs, class codes and subclass codes for PCI devices
void pci_scan_devices();
void pci_list_devices();
void pci_claim_device(struct pci_device_info *pci_device, void (*device_interrupt_handler)());
