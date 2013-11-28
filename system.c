#include "system.h"

void *memmove(void *dest, const void *src, size_t count)
{
	const uint8_t *s = (const uint8_t *) src;
	uint8_t *d = (uint8_t *) dest;

	while(count--) *d++ = *s++;

	return dest;
}

void *memset(void *dest, int value, size_t num)
{
	uint8_t *d = (uint8_t *) dest;
	uint8_t v = (uint8_t) (value & 0xff);

	while(num--) *d++ = v;

	return dest;
}

void *memset_w(void *dest, int value, size_t num)
{
	uint16_t *d = (uint16_t *) dest;
	uint16_t v = (uint16_t) (value & 0xffff);

	while(num--) *d++ = v;

	return dest;
}

void *memset_dw(void *dest, int value, size_t num)
{
	uint32_t *d = (uint32_t *) dest;
	uint32_t v = (uint32_t) value;

	while(num--) *d++ = v;

	return dest;
}

uint8_t inportb(uint16_t _port)
{
	uint8_t rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

uint16_t inportw(uint16_t _port)
{
	uint16_t rv;
	__asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

void outportb(uint16_t _port, uint8_t _data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void int_disable(void)
{
	__asm__ __volatile__ ("cli");
}

void int_enable(void)
{
	__asm__ __volatile__ ("sti");
}

