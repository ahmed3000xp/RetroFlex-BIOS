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

#include "util.h"

void memset(void* buffer, uint8_t val, uint32_t count){
    uint8_t *buf = (uint8_t*)buffer;
    for(uint32_t i = 0; i < count; i++){
        buf[i] = val;
    }
}

void *memcpy(void* dest, const void* src, uint32_t count){
    uint8_t* dest_buf = (uint8_t*)dest;
    const uint8_t* src_buf = (const uint8_t*)src;
    for(uint32_t i = 0; i < count; i++){
        dest_buf[i] = src_buf[i];
    }
    return dest;
}

uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile (
        "inb %1, %0"
        : "=a" (value)      // Output: value in AL
        : "Nd" (port)      // Input: port number
    );
    return value;
}

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile (
        "outb %0, %1"
        :                   // No outputs
        : "a" (value), "Nd" (port) // Inputs: value in AL, port number
    );
}

uint16_t inw(uint16_t port) {
    uint16_t value;
    __asm__ volatile (
        "inw %1, %0"
        : "=a" (value)      // Output: value in AX
        : "Nd" (port)      // Input: port number
    );
    return value;
}

void outw(uint16_t port, uint16_t value) {
    __asm__ volatile (
        "outw %0, %1"
        :                   // No outputs
        : "a" (value), "Nd" (port) // Inputs: value in AX, port number
    );
}

uint32_t indw(uint16_t port) {
    uint32_t value;
    __asm__ volatile (
        "inl %1, %0"
        : "=a" (value)      // Output: value in EAX
        : "Nd" (port)      // Input: port number
    );
    return value;
}

void outdw(uint16_t port, uint32_t value) {
    __asm__ volatile (
        "outl %0, %1"
        :                   // No outputs
        : "a" (value), "Nd" (port) // Inputs: value in EAX, port number
    );
}

void io_wait(){
    outb(0x80, 0);
}

unsigned long long __udivdi3(unsigned long long a, unsigned long long b) {
    unsigned long long quotient = 0;
    while (a >= b) {
        a -= b;
        quotient++;
    }
    return quotient;
}

unsigned long long __umoddi3(unsigned long long a, unsigned long long b) {
    while (a >= b) {
        a -= b;
    }
    return a;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num) {
    const unsigned char *p1 = (const unsigned char *)ptr1;
    const unsigned char *p2 = (const unsigned char *)ptr2;

    for (size_t i = 0; i < num; i++) {
        if (p1[i] != p2[i]) {
            return (int)p1[i] - (int)p2[i];
        }
    }
    return 0;
}

