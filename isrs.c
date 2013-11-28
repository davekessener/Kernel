#include "isrs.h"

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

isr_ptr_t interrupt_handlers[32];

void isrs_install(void)
{
	memset(interrupt_handlers, 0, 32 * sizeof(isr_ptr_t));

#define IDTSG(n) idt_setGate(n,(uint32_t)isr##n,0x08,0x8e)
	IDTSG(0);
	IDTSG(1);
	IDTSG(2);
	IDTSG(3);
	IDTSG(4);
	IDTSG(5);
	IDTSG(6);
	IDTSG(7);
	IDTSG(8);
	IDTSG(9);
	IDTSG(10);
	IDTSG(11);
	IDTSG(12);
	IDTSG(13);
	IDTSG(14);
	IDTSG(15);
	IDTSG(16);
	IDTSG(17);
	IDTSG(18);
	IDTSG(19);
	IDTSG(20);
	IDTSG(21);
	IDTSG(22);
	IDTSG(23);
	IDTSG(24);
	IDTSG(25);
	IDTSG(26);
	IDTSG(27);
	IDTSG(28);
	IDTSG(29);
	IDTSG(30);
	IDTSG(31);
#undef IDTSG
}

char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isrs_installInterruptHandler(size_t i, isr_ptr_t handler)
{
	interrupt_handlers[i] = handler;
}

void fault_handler(struct _sys_regs *r)
{
	int_disable();

    if (r->int_no < 32)
    {
		if(interrupt_handlers[r->int_no])
		{
			interrupt_handlers[r->int_no](r);
		}
		else
		{
			vga_puts("Unhandled ");
        	vga_puts(exception_messages[r->int_no]);
        	vga_puts(" Exception. System Halted!\n");

        	for (;;);
		}
    }
	else
	{
		irq_handler(r);
	}

	int_enable();
}

