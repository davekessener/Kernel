#ifndef __ISRS_H
#define __ISRS_H

#include "system.h"
#include "irq.h"
#include "vga.h"

typedef void (*isr_ptr_t)(sys_regs_t *);

void isrs_install();
void isrs_installInterruptHandler(size_t, isr_ptr_t);

#endif

