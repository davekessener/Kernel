#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static uint16_t * const VGA = (uint16_t *) 0xB8000;
static const uint8_t VGA_H = 24, VGA_W = 80;
static const uint16_t VGAC = 0x0200;
static uint8_t trow, tcol;

void putch(char c)
{
	VGA[trow * VGA_W + tcol] = VGAC | (uint16_t)c;
	if(++tcol == VGA_W)
	{
		tcol = 0;
		if(++trow == VGA_H)
		{
			trow = 0;
		}
	}
}

void t_init()
{
	uint8_t i, j;

	trow = tcol = 0;

	for(i = 0 ; i < VGA_H ; i++)
	{
		for(j = 0 ; j < VGA_W ; j++)
		{
			VGA[i * VGA_W + j] = VGAC | (uint16_t)' ';
		}
	}
}

void puts(const char *str)
{
	while(*str)
	{
		putch(*str++);
	}
}

void main(void)
{
	t_init();
	puts("Hello world!");
}

