#ifndef __VGA_H
#define __VGA_H

#include "system.h"
#include "vsnprintf.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_I_CSR_HI 0x0e
#define VGA_I_CSR_LO 0x0f

void vga_init();
void vga_putch(char);
void vga_puts(const char *);
void vga_puts_dec(uint32_t);
void vga_puts_hex(uint32_t);
void vga_printf(const char *, ...);
void vga_vprintf(const char *, va_list);
void vga_cls();
void vga_setCursor(uint32_t, uint32_t);
void vga_setTextColor(int8_t, uint8_t);
void vga_scroll();

#endif

