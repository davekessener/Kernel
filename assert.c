#include "assert.h"

void panic(const char *msg, const char *fn, uint32_t l)
{
	int_disable();

	vga_puts("!PANIC(\"");
	vga_puts(msg);
	vga_puts("\") at ");
	vga_puts(fn);
	vga_puts(":");
	vga_puts_dec(l);
	vga_puts("!\n");

	while(1);
}

void panic_assert(const char *fn, uint32_t l, const char *s)
{
	int_disable();

	vga_puts("!ASSERT(\"");
	vga_puts(s);
	vga_puts("\") at ");
	vga_puts(fn);
	vga_puts(":");
	vga_puts_dec(l);
	vga_puts(" failed!\n");

	while(1);
}

