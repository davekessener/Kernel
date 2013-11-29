#ifndef __STRING_H
#define __STRING_H

#include "system.h"
#include "kmalloc.h"

size_t strlen(const char *);
void strcpy(char *, const char *);
void strncpy(char *, const char *, size_t);
char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);
char *strdup(const char *);
int strcmp(const char *, const char *);

#endif

