/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: Timer driver
*
*  Notes: No warranty expressed or implied. Use at own risk. */
#include "system.h"

/* This will keep track of how many ticks that the system
*  has been running for */
uint64_t timer_ticks = 0;

void set_timer_phase(int hz)
{
    int divisor = 1193180 / hz;
    outportb(0x43, 0x36);
    outportb(0x40, divisor & 0xff);
    outportb(0x40, (divisor >> 8) & 0xff);
}


/* Handles the timer. In this case, it's very simple: We
*  increment the 'timer_ticks' variable every time the
*  timer fires. By default, the timer fires 18.222 times
*  per second. Why 18.222Hz? Some engineer at IBM must've
*  been smoking something funky */
void timer_handler(struct regs *r)
{
    /* Increment our 'tick count' */
    timer_ticks++;

    /* Every 18 clocks (approximately 1 second), we will
    *  display a message on the screen */
//    if (timer_ticks % 100 == 0)
//    {
//        puts("One second has passed\n");
//    }
}

/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int ticks)
{
    uint64_t eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void timer_install()
{
	set_timer_phase(100);
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
}
