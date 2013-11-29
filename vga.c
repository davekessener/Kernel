#include "vga.h"

#define MKE(ch,c) (((ch)&0xff)|(((c)&0xff)<<8))
#define MKE_STD(ch) MKE(ch,vga_stdcolor)

static uint16_t * const VGA = (uint16_t *) 0xB8000;
static uint16_t vga_csr_x = 0, vga_csr_y = 0;
static uint8_t vga_stdcolor = 0x0f;

void vga_scroll(void)
{
	memmove(VGA, VGA + VGA_WIDTH, VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(uint16_t));
	memset_w(VGA + VGA_WIDTH * (VGA_HEIGHT - 1), MKE_STD(' '), VGA_WIDTH);
	vga_csr_y = vga_csr_y <= 1 ? 0 : vga_csr_y - 1;
}

void vga_updateCursor(void)
{
	vga_setCursor(vga_csr_x, vga_csr_y);
}

void vga_setCursor(uint32_t csr_x, uint32_t csr_y)
{
	uint16_t w = (uint16_t) (csr_y * VGA_WIDTH + csr_x);

	outportb(PORT_VGA_CRT,  VGA_I_CSR_HI);
	outportb(PORT_VGA_DATA, (w >> 8) & 0xff);
	outportb(PORT_VGA_CRT,  VGA_I_CSR_LO);
	outportb(PORT_VGA_DATA,  w       & 0xff);
}

void vga_cls(void)
{
	memset_w(VGA, MKE_STD(' '), VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t));
	vga_setCursor(vga_csr_x = 0, vga_csr_y = 0);
}

void vga_putch(char c)
{
	switch(c)
	{
		case '\b':
			if(vga_csr_x) vga_csr_x--;
			break;
		case '\t':
			vga_csr_x = (vga_csr_x + 8) & ~7;
			break;
		case '\r':
			vga_csr_x = 0;
			break;
		case '\n':
			vga_csr_x = 0;
			vga_csr_y++;
			break;
		default:
			if(c >= ' ')
			{
				VGA[vga_csr_y * VGA_WIDTH + vga_csr_x] = MKE_STD(c);
				vga_csr_x++;
			}
			break;
	}

	if(vga_csr_x >= VGA_WIDTH)
	{
		vga_csr_x = 0;
		vga_csr_y++;
	}

	while(vga_csr_y >= VGA_HEIGHT - 1)
	{
		vga_scroll();
	}

	vga_updateCursor();
}

void vga_puts(const char *str)
{
	while(*str)
	{
		vga_putch(*str++);
	}
}

void vga_puts_num(uint32_t v, uint32_t base)
{
	const char digits[] = "0123456789abcdef";
	uint32_t i = 1;

	if(base <= 1) base = 10;

	while(v / i > 0)
	{
		if(i * base == 0)
		{
			vga_putch(digits[v / i]);
			v -= (v / i) * i;
			break;
		}
		i *= base;
	}

	while(i > 1)
	{
		i /= base;
		if(i == 0) break;

		vga_putch(digits[v / i]);
		v -= (v / i) * i;
	}
}
void vga_puts_dec(uint32_t v) { vga_puts_num(v, 10); }
void vga_puts_hex(uint32_t v) { vga_puts_num(v, 16); }

void vga_printf(const char *str, ...)
{
	va_list params;
	char buf[VGA_WIDTH * VGA_HEIGHT];

	va_start(params, str);
	vsnprintf(buf, sizeof(buf) - 1, str, params);
	va_end(params);

	buf[sizeof(buf) - 1] = '\0';
	vga_puts(buf);
}

void vga_vprintf(const char *str, va_list params)
{
	char buf[VGA_WIDTH * VGA_HEIGHT];

	vsnprintf(buf, sizeof(buf) - 1, str, params);

	buf[sizeof(buf) - 1] = '\0';
	vga_puts(buf);
}

void vga_setTextColor(int8_t fc, uint8_t bc)
{
	vga_stdcolor = ((bc & 0x0f) << 4) | (fc & 0x0f);
}

void vga_init(void)
{
	vga_cls();
}

