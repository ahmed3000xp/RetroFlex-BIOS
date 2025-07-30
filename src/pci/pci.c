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

#include "pci.h"

uint32_t pci_devices_headers[PCI_MAX_DEVICES][sizeof(struct pci_header_type_2)];
struct pci_device_info pci_devices[PCI_MAX_DEVICES];
struct pci_device_info *pci_devices_irqs[16][5] = {{0, 0, 0, 0, 0}};
uint32_t pci_current_mem_bar_address = 0xfff00000;
uint64_t pci_current_mem_bar_address_64 = 0xfff0000000000000;
uint16_t pci_current_io_bar_address = 0xffff;
bool pci_first_detected_ata_controller_setup = false;

bool pci_init(){
    puts("Setting up PCI\n");

    memset(pci_devices_irqs, 0, sizeof(pci_devices_irqs));
    memset(pci_devices, 0, sizeof(pci_devices));
    memset(pci_devices_headers, 0, sizeof(pci_devices_headers));

    if((pci_config_read(0, 0, 0, 0) & 0xffff) == 0xffff){
        outw(0x80, PCI_POST_CODE_DIDNT_DETECT_CONTROLLER);
        puts("PCI Controller was not detected\n");
        return false;
    }
    outw(0x80, PCI_POST_CODE_SCANNING_DEVICES);
    pci_scan_devices();

    outw(0x80, PCI_POST_CODE_DETECTED_CONTROLLER);
    puts("PCI Controller was detected\n");
    return true;
}

uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg_offset){
    uint32_t lbus  = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)func;
  
    outdw(PCI_CONFIG_ADDRESS_PORT, (uint32_t)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (reg_offset & 0xfc) | (0x80000000)));

    return indw(PCI_CONFIG_DATA_PORT);
}

void pci_config_write(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg_offset, uint32_t data){
    uint32_t lbus  = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)func;
  
    outdw(PCI_CONFIG_ADDRESS_PORT, (uint32_t)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (reg_offset & 0xfc) | (0x80000000)));

    outdw(PCI_CONFIG_DATA_PORT, data);
}

void pci_read_header(uint8_t bus, uint8_t device, uint8_t func, struct pci_device_info *pci_device){
    struct pci_header_type_0 *pci_device_header = (struct pci_header_type_0 *)pci_device->device_header_data;
    for(uint8_t i = 0; i < 0x3c; i+=4){
        pci_device->device_header_data[i / 4] = pci_config_read(bus, device, func, i);
    }
    if(pci_device_header->header_type == 0x2){
       for(uint8_t i = 0x3c; i < 0x44; i+=4){
            pci_device->device_header_data[i / 4] = pci_config_read(bus, device, func, i);
       }
    }
    if(pci_device_header->interrupt_line != 0xff && pci_device_header->interrupt_pin == 0){
        pci_devices_irqs[pci_device_header->interrupt_line][4] = pci_device; // This is just for edge cases were the device doesn't have a Interrupt PIN assigned
    }
    else if((pci_device_header->interrupt_line & 0xff) != 0xff){
        pci_devices_irqs[pci_device_header->interrupt_line][pci_device_header->interrupt_pin - 1] = pci_device; // This is just for edge cases were the device doesn't have a Interrupt PIN assigned
    }
    pci_device->device_bus = bus;
    pci_device->device_slot = device;
    pci_device->device_func = func;
    pci_device->device_type = pci_device_header->header_type;
    pci_device->device_unknown = true; // By default the kernel will mark it as true but if a driver in the kernel or a module claimed it, the kernel will set it to false
    pci_device->vendor_id = pci_device_header->vendor_id;
    pci_device->device_id = pci_device_header->device_id;
    if(pci_device->vendor_id == 0x10de){
        pci_device->device_nvidia = true; // Device is from NVIDIA, uh oh......
    }
    pci_device->interrupt_pin = pci_device_header->interrupt_pin;
    pci_device->interrupt_line = pci_device_header->interrupt_line;
    pci_device->device_interrupt_handler = NULL; // By default it will be NULL, unless a driver claimed the device and assigned an interrupt handler
}

void pci_setup_device(uint8_t bus, uint8_t device, uint8_t func, struct pci_device_info *pci_device){
    struct pci_header_type_0 *pci_device_header = (struct pci_header_type_0 *)pci_device->device_header_data;
    uint64_t old_bar_value;
    uint64_t calculated_bar_size;
    bool device_setup = true;
    
    for(uint8_t i = 0; i < 0x3c; i+=4){
        pci_device->device_header_data[i / 4] = pci_config_read(bus, device, func, i);
    }
    
    struct pci_memory_bar *pci_memory_bar = (struct pci_memory_bar *)&pci_device_header->bar0;
    struct pci_io_bar *pci_bar_io = (struct pci_io_bar *)&pci_device_header->bar0;
    (void)pci_bar_io; // Might be used in the future
    if(pci_device_header->header_type == 0x2){
        for(uint8_t i = 0x3c; i < 0x44; i+=4){
            pci_device->device_header_data[i / 4] = pci_config_read(bus, device, func, i);
        }
        pci_config_write(bus, device, func, 4, (pci_config_read(bus, device, func, 4) & 0xfffc)); // Disable Memory and IO decoding
        device_setup = false;
        // The BIOS will not setup PCI-to-CardBus bridges for the time being
    }
    else if(pci_device_header->header_type == 0x1){
        pci_config_write(bus, device, func, 4, (pci_config_read(bus, device, func, 4) & 0xfffc)); // Disable Memory and IO decoding
        device_setup = false;
        // The BIOS will not setup PCI-to-PCI bridges for the time being
    }
    else if(pci_device_header->header_type == 0){
        for(uint8_t i = 0; i < 6; i++){
            switch(pci_memory_bar[i].addressing_type){
                case PCI_BAR_ADDRESSING_TYPE_MEMORY:
                    switch(pci_memory_bar[i].bar_type){
                        case PCI_MEMORY_BAR_TYPE_16_BIT:
                            outw(0x80, PCI_POST_CODE_UNSUPPORTED_BAR);
                            device_setup = false;
                            printf("Unsupported PCI BAR detected, Memory/IO %d, BAR Type 0x%x, Address 0x%x. ", pci_memory_bar[i].addressing_type, pci_memory_bar[i].bar_type, pci_memory_bar[i].bar_address);
                            break;
                        case PCI_MEMORY_BAR_TYPE_64_BIT:
                            outw(0x80, PCI_POST_CODE_MAPPING_64_BIT_BAR);
                            if(i + 1 >= 6){
                                continue;
                            }
                            pci_config_write(bus, device, func, 4, (pci_config_read(bus, device, func, 4) & 0xfffc)); // Disable Memory and IO decoding
                            
                            old_bar_value = (uint64_t)pci_config_read(bus, device, func, 0x10 + i * 4);
                            old_bar_value |= (uint64_t)((uint64_t)pci_config_read(bus, device, func, 0x10 + (i + 1) * 4) << 32);
                            if((old_bar_value & 0xfffffffffffffff0) == 0)
                                continue;

                            pci_config_write(bus, device, func, 0x10 + i * 4, 0xffffffff);
                            pci_config_write(bus, device, func, 0x10 + (i + 1) * 4, 0xffffffff);

                            calculated_bar_size = (uint64_t)(pci_config_read(bus, device, func, 0x10 + i * 4) & 0xfffffff0);
                            calculated_bar_size |= (uint64_t)((uint64_t)pci_config_read(bus, device, func, 0x10 + (i + 1) * 4) << 32);

                            pci_config_write(bus, device, func, 0x10 + i * 4, (uint32_t)(old_bar_value & 0xffffffff));
                            pci_config_write(bus, device, func, 0x10 + (i + 1) * 4, (uint32_t)((old_bar_value >> 32) & 0xffffffff));

                            calculated_bar_size = (~calculated_bar_size + 1);

                            pci_current_mem_bar_address_64 -= calculated_bar_size;
                            pci_config_write(bus, device, func, 0x10 + i * 4, (uint32_t)(pci_current_mem_bar_address_64 & 0xfffffff0));
                            pci_config_write(bus, device, func, 0x10 + (i + 1) * 4, (uint32_t)((pci_current_mem_bar_address_64 >> 32) & 0xffffffff));

                            pci_config_write(bus, device,func, 4, ((pci_config_read(bus, device, func, 4) & 0xfffc) + 3)); // Enable Memory and IO decoding
                            i++;
                            break;
                        case PCI_MEMORY_BAR_TYPE_32_BIT:
                            outw(0x80, PCI_POST_CODE_MAPPING_32_BIT_BAR);
                            pci_config_write(bus, device, func, 4, (pci_config_read(bus, device, func, 4) & 0xfffc)); // Disable Memory and IO decoding
                            
                            old_bar_value = pci_config_read(bus, device, func, 0x10 + i * 4);
                            if((old_bar_value & 0xfffffff0) == 0)
                                continue;

                            pci_config_write(bus, device, func, 0x10 + i * 4, 0xffffffff);

                            calculated_bar_size = (pci_config_read(bus, device, func, 0x10 + i * 4) & 0xfffffff0);

                            pci_config_write(bus, device, func, 0x10 + i * 4, (uint32_t)old_bar_value);

                            calculated_bar_size = (~calculated_bar_size + 1);

                            pci_current_mem_bar_address -= calculated_bar_size;
                            pci_config_write(bus, device, func, 0x10 + i * 4, (pci_current_mem_bar_address & 0xfffffff0));

                            pci_config_write(bus, device, func, 4, ((pci_config_read(bus, device, func, 4) & 0xfffc) + 3)); // Enable Memory and IO decoding
                            break;
                        default:
                            device_setup = false;
                            break;
                    }
                    break;
                case PCI_BAR_ADDRESSING_TYPE_IO:
                    outw(0x80, PCI_POST_CODE_MAPPING_IO_BAR);
                    pci_config_write(bus, device, func, 4, (pci_config_read(bus, device, func, 4) & 0xfffc)); // Disable Memory and IO decoding
                            
                    old_bar_value = pci_config_read(bus, device, func, 0x10 + i * 4);
                    if((old_bar_value & 0xfffffffc) == 0)
                        continue;

                    pci_config_write(bus, device, func, 0x10 + i * 4, 0xffffffff);

                    calculated_bar_size = (pci_config_read(bus, device, func, 0x10 + i * 4) & 0xfffffffc);

                    pci_config_write(bus, device, func, 0x10 + i * 4, old_bar_value);

                    calculated_bar_size = (~calculated_bar_size + 1);

                    pci_current_io_bar_address -= calculated_bar_size;
                    pci_config_write(bus, device, func, 0x10 + i * 4, (pci_current_io_bar_address & 0xfffc));

                    pci_config_write(bus, device, func, 4, ((pci_config_read(bus, device, func, 4) & 0xfffc) + 3)); // Enable Memory and IO decoding
                    break;
            }
        }
        bool device_has_rom = true;
        pci_config_write(bus, device, func, 4, (pci_config_read(bus, device, func, 4) & 0xfffc)); // Disable Memory and IO decoding
                            
        old_bar_value = pci_config_read(bus, device, func, 0x30);

        pci_config_write(bus, device, func, 0x30, 0xfffff800);

        calculated_bar_size = (pci_config_read(bus, device, func, 0x30) & 0xfffff800);
        if((calculated_bar_size & 0xfffff800) == 0){
            printf("PCI Device %x:%x.%x doesn't have an Expansion ROM\n", bus, device, func);
            device_has_rom = false;
        }

        if(device_has_rom){
            pci_config_write(bus, device, func, 0x30, (uint32_t)old_bar_value);

            calculated_bar_size = (~calculated_bar_size + 1);

            pci_current_mem_bar_address -= calculated_bar_size;
            pci_current_mem_bar_address &= 0xfffff800;
            pci_current_mem_bar_address -= 0x800;
            pci_config_write(bus, device, func, 0x30, (pci_current_mem_bar_address & 0xfffff800));
        }
        
        pci_config_write(bus, device, func, 4, ((pci_config_read(bus, device, func, 4) & 0xfffc) + 3)); // Enable Memory and IO decoding

        pci_read_header(bus, device, func, pci_device);
        pci_init_device(bus, device, func);
    }
    if(!device_setup)
        printf("Didn't setup device %x:%x.%x, Type %x [%x:%x]\n", bus, device, func, pci_device_header->header_type, pci_device_header->vendor_id, pci_device_header->device_id);
}

void pci_init_device(uint8_t bus, uint8_t device, uint8_t func){ // This function supports only specific Prog IFs, class codes and subclass codes for PCI devices
    uint32_t class_code = (pci_config_read(bus, device, func, 8) & 0xffffff00);
    bool device_has_rom = true;
    uint32_t old_bar_value;
    uint32_t rom_bar;

    switch(class_code){
        case 0x03000000:
            pci_config_write(bus, device, func, 4, (pci_config_read(bus, device, func, 4) & 0xfffc)); // Disable Memory and IO decoding
            old_bar_value = pci_config_read(bus, device, func, 0x30);

            pci_config_write(bus, device, func, 0x30, 0xfffff800);

            rom_bar = (pci_config_read(bus, device, func, 0x30) & 0xfffff800);
            if((rom_bar & 0xfffff800) == 0){
                printf("PCI VGA Device %x:%x.%x doesn't have an Option ROM\n", bus, device, func);
                device_has_rom = false;
            }

            if(device_has_rom){
                pci_config_write(bus, device, func, 0x30, (uint32_t)old_bar_value);

                rom_bar = 0xc0000;

                pci_config_write(bus, device, func, 0x30, (rom_bar & 0xfffff800));
            }
        
            pci_config_write(bus, device, func, 0x30, (pci_config_read(bus, device, func, 0x30) & 0xfffffffe) + 1);
            pci_config_write(bus, device, func, 4, ((pci_config_read(bus, device, func, 4) & 0xfffc) + 3)); // Enable Memory and IO decoding
            break;
        default:
            printf("Unsupported PCI Device %x:%x.%x for init\n", bus, device, func);
            break;
    }
}

void pci_scan_devices(){
    uint8_t pci_device_index = 0;
    for (uint8_t bus = 0; bus < 255; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint32_t vendor_device = pci_config_read(bus, device, func, 0);
                uint16_t vendor_id = vendor_device & 0xffff;
                if (vendor_id == 0xffff) {
                    if (func == 0) {
                        break;
                    } else {
                        continue;
                    }
                }

                pci_devices[pci_device_index].device_header_data = (uint32_t *)&pci_devices_headers[pci_device_index];
                pci_setup_device(bus, device, func, &pci_devices[pci_device_index]);
                pci_read_header(bus, device, func, &pci_devices[pci_device_index]);
                outw(0x80, PCI_POST_CODE_DETECTED_DEVICE);
                pci_device_index++;
                if(pci_device_index == PCI_MAX_DEVICES){
                    return;
                }
                
                uint32_t header_type_reg = pci_config_read(bus, device, func, 0xc);
                uint8_t header_type = (header_type_reg >> 16) & 0xff;

                if (func == 0 && !(header_type & 0x80)) {
                    break;
                }
            }
        }
    }
}


void pci_list_devices(){
    for(uint8_t i = 0; i < 70; i++){
        if(pci_devices[i].device_id == 0 && pci_devices[i].vendor_id == 0)
            return;
        printf("%x:%x.%x Device: Unknown PCI device [%x:%x]\n\tDevice Type 0x%x\n", pci_devices[i].device_bus, pci_devices[i].device_slot, pci_devices[i].device_func, pci_devices[i].vendor_id, pci_devices[i].device_id, pci_devices[i].device_type);
    }
}

void pci_claim_device(struct pci_device_info *pci_device, void (*device_interrupt_handler)()){
    pci_device->device_unknown = false;
    pci_device->device_interrupt_handler = device_interrupt_handler;
}
