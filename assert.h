#ifndef __ASSERT_H
#define __ASSERT_H

#include "system.h"
#include "vga.h"

#define PANIC(s) panic(s,__FILE__,__LINE__)
#define ASSERT(v) ((v) ? 0 : panic_assert(__FILE__,__LINE__,#v))

void panic(const char *, const char *, uint32_t);
void panic_assert(const char *, uint32_t, const char *);

#endif

