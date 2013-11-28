#include "timer.h"

uint64_t timer_ticks = 0;

void set_timer_phase(int hz)
{
    int divisor = 1193180 / hz;

    outportb(PORT_CLCK_CRT,  0x36);
    outportb(PORT_CLCK_DATA,  divisor       & 0xff);
    outportb(PORT_CLCK_DATA, (divisor >> 8) & 0xff);
}

void timer_handler(struct _sys_regs *r)
{
	timer_ticks++;
}

void timer_wait(uint32_t ticks)
{
	uint64_t eticks = timer_ticks + ticks;

	while(timer_ticks < eticks);
}

void timer_install(void)
{
	set_timer_phase(100);

	irq_installHandler(INT_CLOCK, timer_handler);
}

