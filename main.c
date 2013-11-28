#include "system.h"
#include "gdt.h"
#include "idt.h"
#include "isrs.h"
#include "irq.h"
#include "vga.h"
#include "timer.h"
#include "keyboard.h"
#include "paging.h"
#include "kmalloc.h"

void main()
{
	gdt_install();
	idt_install();
	isrs_install();
	irq_install();
	vga_init();
	timer_install();
	keyboard_install();
	paging_init();
	
	int_enable();
	
	vga_puts("Hello World!\n");

	while(1);
}

