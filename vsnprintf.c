#include "vsnprintf.h"

#define PF_NONE 0
#define PF_LEFT 1
#define PF_SIGN 2
#define PF_SPACE 4
#define PF_SPECIAL 8
#define PF_ZERO 16
#define PF_UPC 32
#define PF_SIGNED 64

static size_t iaddr(char *, int, unsigned char *, int, int, int);
static size_t eaddr(char *, int, unsigned char *, int, int, int);
static int is_digit(char);
static size_t strnlen(const char *, size_t);
static int atoi_skip(const char **);
static size_t number(char *, int, long, int, int, int, int);

int vsnprintf(char *str, size_t n, const char *fmt, va_list params)
{
	unsigned int flags;
	int pr, qu, fw, base, len;
	unsigned long num;
	size_t i, j;
	char *s;
	size_t (*addr)(char *, int, unsigned char *, int, int, int);

	for(i = 0 ; i < n ; i++)
	{
		if(!*fmt) break;

		if(*fmt != '%')
		{
			str[i] = *fmt++;
			continue;
		}

		flags = PF_NONE;
formatloop:
		if(!*++fmt) return -1;
		switch(*fmt)
		{
			case '-': flags |= PF_LEFT;    goto formatloop;
			case '+': flags |= PF_SIGN;    goto formatloop;
			case ' ': flags |= PF_SPACE;   goto formatloop;
			case '#': flags |= PF_SPECIAL; goto formatloop;
			case '0': flags |= PF_ZERO;    goto formatloop;
		}

		fw = -1;
		if(is_digit(*fmt))
		{
			fw = atoi_skip(&fmt);
		}
		else if(*fmt == '*')
		{
			++fmt;
			fw = va_arg(params, int);

			if(fw < 0)
			{
				fw = -fw;
				flags |= PF_LEFT;
			}
		}

		pr = -1;
		if(*fmt == '.')
		{
			++fmt;

			if(is_digit(*fmt))
			{
				pr = atoi_skip(&fmt);
			}
			else if(*fmt == '*')
			{
				++fmt;
				pr = va_arg(params, int);
			}

			if(pr < 0) pr = 0;
		}

		qu = -1;
		if(*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
		{
			qu = *fmt++;
		}

		base = 10;

		switch(*fmt)
		{
			case 'c':
				if(!(flags & PF_LEFT)) 
					while(--fw > 0) 
					{
						str[i++] = ' ';
						if(i == n) return -1;
					}

				str[i] = (unsigned char) va_arg(params, int);

				while(--fw > 0)
				{
					if(++i == n) return -1;
					str[i] = ' ';
				}
				fmt++;
				continue;
			case 's':
				s = va_arg(params, char *);
				if(!s) s = "<NULL>";
				len = strnlen(s, pr);
				if(!(flags & PF_LEFT))
					while(len < fw--)
					{
						str[i++] = ' ';
						if(i == n) return -1;
					}

				for(j = 0 ; j < len ; j++)
				{
					str[i++] = *s++;
					if(i == n) return -1;
				}
				--i;
				while(len < fw--)
				{
					if(++i == n) return -1;
					str[i] = ' ';
				}
				fmt++;
				continue;
			case 'p':
				if(fw == -1)
				{
					fw = 2 * sizeof(void *);
					flags |= PF_ZERO;
				}

				if((i += number(str + i, n - i, (uint32_t) va_arg(params, void *), 16, fw, pr, flags)) == n)
				{
					return -1;
				}
				--i;
				fmt++;
				continue;
			case 'n':
				if(qu == 'l')
				{
					long *ip = va_arg(params, long *);
					*ip = i;
				}
				else
				{
					int *ip = va_arg(params, int *);
					*ip = i;
				}
				fmt++;
				continue;
			case 'A':
				flags |= PF_UPC;
			case 'a':
				addr = (qu == 'l') ? eaddr : iaddr;

				if((i += addr(str + i, n - i, va_arg(params, unsigned char *), fw, pr, flags)) == n)
				{
					return -1;
				}
				--i;
				fmt++;

				continue;
			case 'o':
				base = 8;
				break;
			case 'X':
				flags |= PF_UPC;
			case 'x':
				base = 16;
				break;
			case 'd':
			case 'i':
				flags |= PF_SIGNED;
			case 'u':
				break;
			default:
				if(*fmt != '%') str[i++] = '%';
				if(*fmt)
				{
					str[i] = *fmt;
					fmt++;
				}
				continue;
		}

		if(qu == 'l')
		{
			num = va_arg(params, unsigned long);
		}
		else if(qu == 'h')
		{
			if(flags & PF_SIGNED)
			{
				num = (short) va_arg(params, int);
			}
			else
			{
				num = (unsigned short) va_arg(params, unsigned int);
			}
		}
		else if(flags & PF_SIGNED)
		{
			num = va_arg(params, int);
		}
		else
		{
			num = va_arg(params, unsigned int);
		}

		if((i += number(str + i, n - i, num, base, fw, pr, flags)) == n)
		{
			return -1;
		}
		--i;
		fmt++;
	}

	if(i == n) return -1;

	str[i] = '\0';

	return i;
}

int snprintf(char *str, size_t n, const char *fmt, ...)
{
	int l;
	va_list params;

	va_start(params, fmt);
	l = vsnprintf(str, n, fmt, params);
	va_end(params);

	return l;
}

int vsprintf(char *str, const char *fmt, va_list params)
{
	return vsnprintf(str, SIZE_MAX, fmt, params);
}

int sprintf(char *str, const char *fmt, ...)
{
	int l;
	va_list params;

	va_start(params, fmt);
	l = vsnprintf(str, SIZE_MAX, fmt, params);
	va_end(params);

	return l;
}

// # ===========================================================================

const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
const char digits_c[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static int is_digit(char c) { return c >= '0' && c <= '9'; }

static int atoi_skip(const char **str)
{
	int r = 0;

	while(is_digit(**str)) r = (10 * r) + *((*str)++) - '0';

	return r;
}

static size_t strnlen(const char *s, size_t n)
{
	const char *str = s;
	while(n-- && *str) str++;
	return str - s;
}

static size_t number(char *str, int orig_n, long num, int base, int size, int precision, int type)
{
	const char *dig = (type & PF_UPC) ? digits_c : digits;
	char c, sign, tmp[128];
	int i, n = orig_n;

	if(type & PF_LEFT) type &= ~PF_ZERO;
	if(base < 2 || base > 36) base = 10;

	c = (type & PF_ZERO) ? '0' : ' ';

	sign = 0;
	if(type & PF_SIGNED)
	{
		if(num < 0)
		{
			sign = '-';
			num = -num;
			size--;
		}
		else if(type & PF_SIGN)
		{
			sign = '+';
			size--;
		}
		else if(type & PF_SPACE)
		{
			sign = ' ';
			size--;
		}
	}

	if(type & PF_SPECIAL)
	{
		if(base == 16)
		{
			size -= 2;
		}
		else if(base == 8)
		{
			--size;
		}
	}

	i = 0;
	if(n <= 0) return 0;

	if(num == 0)
	{
		tmp[i++] = '0';
	}
	else
	{
		while(num != 0)
		{
			tmp[i++] = dig[((unsigned long) num) % (unsigned) base];
			num = ((unsigned long) num) / (unsigned) base;
		}
	}

	if(i > precision) precision = i;
	size -= precision;
	if(!(type & (PF_ZERO | PF_LEFT)))
		while(size-- > 0)
		{
			*str++ = ' ';
			if(--n <= 0) return orig_n;
		}
	
	if(sign)
	{
		*str++ = sign;
		if(--n <= 0) return orig_n;
	}

	if(type & PF_SPECIAL)
	{
		if(base == 8)
		{
			*str++ = '0';
			if(--n <= 0) return orig_n;
		}
		else if(base == 16)
		{
			*str++ = '0';
			if(--n <= 0) return orig_n;
			*str++ = 'x';
			if(--n <= 0) return orig_n;
		}
	}

	if(!(type & PF_LEFT))
		while(size-- > 0)
		{
			*str++ = c;
			if(--n <= 0) return orig_n;
		}
	
	while(i < precision--)
	{
		*str++ = '0';
		if(--n <= 0) return orig_n;
	}
	while(i-- > 0)
	{
		*str++ = tmp[i];
		if(--n <= 0) return orig_n;
	}
	while(size-- > 0)
	{
		*str++ = ' ';
		if(--n <= 0) return orig_n;
	}

	return orig_n - n;
}

static size_t eaddr(char *str, int orig_n, unsigned char *addr, int size, int precision, int type)
{
	char tmp[24];
	const char *dig = (type & PF_UPC) ? digits_c : digits;
	int i, len, n = orig_n;

	len = 0;
	for(i = 0 ; i < 6 ; i++)
	{
		if(i != 0) tmp[len++] = ':';
		tmp[len++] = dig[addr[i] >> 4];
		tmp[len++] = dig[addr[i] & 0x0f];
	}

	if(!(type & PF_LEFT))
		while(len < size--)
		{
			*str++ = ' ';
			if(--n <= 0) return orig_n;
		}
	
	for(i = 0 ; i < len ; i++)
	{
		*str++ = tmp[i];
		if(--n <= 0) return orig_n;
	}

	while(len < size--)
	{
		*str++ = ' ';
		if(--n <= 0) return orig_n;
	}

	return orig_n - n;
}

static size_t iaddr(char *str, int orig_n, unsigned char *addr, int size, int precision, int type)
{
	char tmp[24];
	int i, m, len, n = orig_n;

	len = 0;
	for(i = 0 ; i < 4 ; i++)
	{
		if(i != 0) tmp[len++] = '.';
		m = addr[i];

		if(m >= 100)
		{
			tmp[len++] = digits[m / 100];
			m = m % 100;
		}

		if(m >= 10)
		{
			tmp[len++] = digits[m / 10];
			m = m % 10;
		}

		tmp[len++] = digits[m];
	}

	if(!(type & PF_LEFT))
		while(len < size--)
		{
			*str++ = ' ';
			if(--n <= 0) return orig_n;
		}

	for(i = 0 ; i < len ; ++i)
	{
		*str++ = tmp[i];
		if(--n <= 0) return orig_n;
	}

	while(len < size--)
	{
		*str++ = ' ';
		if(--n <= 0) return orig_n;
	}

	return orig_n - n;
}

