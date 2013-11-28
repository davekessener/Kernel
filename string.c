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


