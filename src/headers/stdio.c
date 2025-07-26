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

#include "stdio.h"

void putc(char c){
    outb(0xe9, c);
}

void printf(const char *fmt, ...){
    int32_t *argp = (int32_t*)&fmt;
    int32_t state = PRINTF_STATE_START;
    int32_t length = PRINTF_LENGTH_START;
    int32_t radix = 10;
    bool sign = false;

    argp++;
    while(*fmt){
        switch(state){
        case PRINTF_STATE_START:
            if(*fmt == '%'){
                state = PRINTF_STATE_LENGTH;
            }
            else{
                putc(*fmt);
            }
            break;
        case PRINTF_STATE_LENGTH:
            if(*fmt == 'h'){
                length = PRINTF_LENGTH_SHORT;
                state = PRINTF_STATE_SHORT;
            }
            else if(*fmt == 'l'){
                length = PRINTF_LENGTH_LONG;
                state = PRINTF_STATE_LONG;
            }
            else{
                goto PRINTF_STATE_SPEC_;
            }
            break;
        case PRINTF_STATE_SHORT:
            if (*fmt == 'h'){
                length = PRINTF_LENGTH_SHORT_SHORT;
                state = PRINTF_STATE_SPEC;
            }
            else{
                goto PRINTF_STATE_SPEC_;
            }
            break;

        case PRINTF_STATE_LONG:
            if(*fmt == 'l'){
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
            }
            else{
                    goto PRINTF_STATE_SPEC_;
            }
            break;

        case PRINTF_STATE_SPEC:
PRINTF_STATE_SPEC_:
                switch(*fmt){
                    case 'c':
                        putc((char)*argp);
                        argp++;
                        break;
                    case 's':
                        if(length == PRINTF_LENGTH_LONG || length == PRINTF_LENGTH_LONG_LONG){
                            puts(*(const char **)argp);
                            argp += 2;
                        }
                        else{
                            puts(*(const char **)argp);
                            argp++;
                        }
                        break;
                    case '%':
                        putc('%');
                        break;
                    case 'd':
                    case 'i':
                        radix = 10;
                        sign = true;
                        argp = printf_number(argp, length, sign, radix);
                        break;
                    case 'u':
                        radix = 10;
                        sign = false;
                        argp = printf_number(argp, length, sign, radix);
                        break;
                    case 'X':
                    case 'x':
                    case 'p':
                        radix = 16;
                        sign = false;
                        argp = printf_number(argp, length, sign, radix);
                        break;
                    case 'o':
                        radix = 8;
                        sign = false;
                        argp = printf_number(argp, length, sign, radix);
                        break;
                    default:
                        break;

                }
            state = PRINTF_STATE_START;
            length = PRINTF_LENGTH_START;
            radix = 10;
            sign = false;
            break;
            }
        fmt++;
    }    
}

const char hex_chars[] = "0123456789abcdef";

int32_t *printf_number(int32_t *argp, int length, bool sign, int radix){
    char buffer[32] = "";
    uint32_t number = 0;
    int number_sign = 1;
    int pos = 0;

    switch(length){
        case PRINTF_LENGTH_SHORT_SHORT:
        case PRINTF_LENGTH_SHORT:
        case PRINTF_LENGTH_START:
            if (sign){
                int n = *argp;
                if (n < 0){
                    n = -n;
                    number_sign = -1;
                }
                number = (uint32_t)n;
            }
            else{
                number = *(uint32_t*)argp;
            }
            argp++;
            break;
        case PRINTF_LENGTH_LONG:
            if (sign){
                long int n = *(long int*)argp;
                if (n < 0){
                    n = -n;
                    number_sign = -1;
                }
                number = (uint32_t)n;
            }
            else{
                number = *(uint32_t*)argp;
            }
            argp += 2;
            break;
        case PRINTF_LENGTH_LONG_LONG:
            if(sign){
                long long int n = *(long long int*)argp;
                if(n < 0){
                    n = -n;
                    number_sign = -1;
                }
                number = (uint32_t) n;
            }
            else{
                number = *(uint32_t*) argp;
            }
            argp += 4;
            break;
    }

    do {
        uint32_t rem = number % radix;
        number = number / radix;
        
        buffer[pos++] = hex_chars[rem];
    } while (number > 0);

    if(sign && number_sign < 0){
        buffer[pos++] = '-';
    }

    while(--pos >= 0){
        putc(buffer[pos]);
    }

    return argp;
}
