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

uint32_t pci_devices_headers[70][sizeof(struct pci_header_type_2)];
struct pci_device_info pci_devices[70];
struct pci_device_info *pci_devices_irqs[16][5] = {{0, 0, 0, 0, 0}};

bool pci_init(){
    memset(pci_devices_irqs, 0, sizeof(pci_devices_irqs));
    memset(pci_devices, 0, sizeof(pci_devices));
    memset(pci_devices_headers, 0, sizeof(pci_devices_headers));

    pci_setup_device(0, 0, 0, &pci_devices[0]);
    
    if((pci_config_read(0, 0, 0, 0) & 0xffff) == 0xffff)
        return false;
    pci_scan_devices();

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
    (void)pci_device;
    /*struct pci_header_type_0 *pci_device_header = (struct pci_header_type_0 *)pci_device->device_header_data;
    
    for(uint8_t i = 0; i < 0x3c; i+=4){
        pci_device->device_header_data[i / 4] = pci_config_read(bus, device, func, i);
    }
    
    if(pci_device_header->header_type == 0x2){
       for(uint8_t i = 0x3c; i < 0x44; i+=4){
            pci_device->device_header_data[i / 4] = pci_config_read(bus, device, func, i);
       }
       struct pci_header_type_2 *pci_device_cardbus_header = (struct pci_header_type_2 *)pci_device_header;
    }
    else if(pci_device_header->header_type == 0x1){
        struct pci_header_type_1 *pci_device_bridge_header = (struct pci_header_type_1 *)pci_device_header;
    }*/
    printf("Didn't setup device %x:%x.%x\n", bus, device, func);
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
                pci_device_index++;
                if(pci_device_index == 70){
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
        printf("%x:%x.%x Device: Unknown PCI device [%x:%x]\n", pci_devices[i].device_bus, pci_devices[i].device_slot, pci_devices[i].device_func, pci_devices[i].vendor_id, pci_devices[i].device_id);
    }
}

void pci_claim_device(struct pci_device_info *pci_device, void (*device_interrupt_handler)()){
    pci_device->device_unknown = false;
    pci_device->device_interrupt_handler = device_interrupt_handler;
}
