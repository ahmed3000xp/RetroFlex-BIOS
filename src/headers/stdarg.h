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

typedef char* va_list;

#define va_start(ap, last) ((ap) = (va_list)&(last) + sizeof(last))
#define va_arg(ap, type) (*(type*)((ap) += sizeof(type), (ap) - sizeof(type)))
#define va_end(ap) ((ap) = (va_list)0)
