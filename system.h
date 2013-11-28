#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdint.h>

typedef uint32_t size_t;

#define NULL ((void *) 0)
#define PORT_VGA_CRT   0x03d4
#define PORT_VGA_DATA  0x03d5
#define PORT_IRQ_CRT1  0x0020
#define PORT_IRQ_DATA1 0x0021
#define PORT_IRQ_CRT2  0x00a0
#define PORT_IRQ_DATA2 0x00a1
#define PORT_KB_DATA   0x0060
#define PORT_CLCK_CRT  0x0043
#define PORT_CLCK_DATA 0x0040

#define INT_CLOCK		0x00
#define INT_KB			0x01

typedef struct _sys_regs
{
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, useresp, ss;
} sys_regs_t;

void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);
void *memset_w(void *, int, size_t);
void *memset_dw(void *, int , size_t);
uint8_t  inportb(uint16_t);
uint16_t inportw(uint16_t);
void outportb(uint16_t, uint8_t);
void int_disable();
void int_enable();

#endif

