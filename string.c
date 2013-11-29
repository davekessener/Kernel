#include "string.h"

size_t strlen(const char *str)
{
	size_t i = 0;
	while(str[i++]);

	return i - 1;
}

void strcpy(char *dest, const char *src)
{
	while((*dest++ = *src++));
}

void strncpy(char *dest, const char *src, size_t max)
{
	while((*dest++ = *src++ )) if(--max == 0) break;
}

char *strcat(char *dest, const char *src)
{
	while(*dest++);
	dest--;

	while((*dest++ = *src++));

	return dest;
}

char *strncat(char *dest, const char *src, size_t max)
{
	while(*dest++) if(max-- == 0) break;
	dest--;

	while((*dest++ = *src++)) if(--max == 0) break;

	return dest;
}

char *strdup(const char *str)
{
	size_t l = strlen(str);
	char *s = kmalloc(l + 1);

	strncpy(s, str, l);

	s[l] = '\0';

	return s;
}

int strcmp(const char *str1, const char *str2)
{
	const char *c = str1;
	if(!str1 || !str2) return -1;

	do
	{
		if(!*str1 || !*str2)
		{
			break;
		}
	} while(*str1++ == *str2++);
	--str1;
	--str2;

	if(*str1 == *str2 && str1[1] == '\0' && str1[1] == str2[1]) return 0;

	return str1 - c;
}

