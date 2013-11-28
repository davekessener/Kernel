#ifndef __TIMER_H
#define __TIMER_H

#include "system.h"
#include "irq.h"

void timer_wait(uint32_t);
void timer_install();

#endif

