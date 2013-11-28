#ifndef __IRQ_H
#define __IRQ_H

#include "system.h"
#include "idt.h"

void irq_installHandler(uint32_t, void (*)(struct _sys_regs *));
void irq_uninstallHandler(uint32_t);
void irq_handler(struct _sys_regs *);
void irq_install();

#endif

