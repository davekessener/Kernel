#ifndef __VSNPRINTF_H
#define __VSNPRINTF_H

#include <stdarg.h>
#include "system.h"

int sprintf(char *, const char *, ...);
int snprintf(char *, size_t, const char *, ...);
int vsprintf(char *, const char *, va_list);
int vsnprintf(char *, size_t, const char *, va_list);

#endif

