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

#include "stdint.h"

#define CEIL_DIV(a,b) (((a + b) - 1)/b)

void memset(void* buffer, uint8_t val, uint32_t count);
void *memcpy(void* dest, const void* src, uint32_t count);

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t val);

uint32_t indw(uint16_t port);
void outdw(uint16_t port, uint32_t val);

int memcmp(const void *ptr1, const void *ptr2, size_t num);

void io_wait();

unsigned long long __udivdi3(unsigned long long a, unsigned long long b);
unsigned long long __umoddi3(unsigned long long a, unsigned long long b);
