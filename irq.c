#include "irq.h"

#define IRQ_EOI 0x20

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void *irq_routines[16] =
{
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

void irq_remap(void)
{
    outportb(PORT_IRQ_CRT1,  0x11);
    outportb(PORT_IRQ_CRT2,  0x11);
    outportb(PORT_IRQ_DATA1, 0x20);
    outportb(PORT_IRQ_DATA2, 0x28);
    outportb(PORT_IRQ_DATA1, 0x04);
    outportb(PORT_IRQ_DATA2, 0x02);
    outportb(PORT_IRQ_DATA1, 0x01);
    outportb(PORT_IRQ_DATA2, 0x01);
    outportb(PORT_IRQ_DATA1, 0x00);
    outportb(PORT_IRQ_DATA2, 0x00);
}

void irq_installHandler(uint32_t irq, void (*handler)(struct _sys_regs *r))
{
	irq_routines[irq] = handler;
}

void irq_uninstallHandler(uint32_t irq)
{
	irq_routines[irq] = NULL;
}

void irq_install(void)
{	
    irq_remap();

    idt_setGate(32, (uint32_t)irq0,  0x08, 0x8E);
    idt_setGate(33, (uint32_t)irq1,  0x08, 0x8E);
    idt_setGate(34, (uint32_t)irq2,  0x08, 0x8E);
    idt_setGate(35, (uint32_t)irq3,  0x08, 0x8E);
    idt_setGate(36, (uint32_t)irq4,  0x08, 0x8E);
    idt_setGate(37, (uint32_t)irq5,  0x08, 0x8E);
    idt_setGate(38, (uint32_t)irq6,  0x08, 0x8E);
    idt_setGate(39, (uint32_t)irq7,  0x08, 0x8E);

    idt_setGate(40, (uint32_t)irq8,  0x08, 0x8E);
    idt_setGate(41, (uint32_t)irq9,  0x08, 0x8E);
    idt_setGate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_setGate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_setGate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_setGate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_setGate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_setGate(47, (uint32_t)irq15, 0x08, 0x8E);
}

void irq_handler(struct _sys_regs *r)
{
	void (*handler)(struct _sys_regs *) = irq_routines[r->int_no - 32];

	if(handler)
	{
		handler(r);
	}

	if(r->int_no >= 40)
	{
		outportb(PORT_IRQ_CRT2, IRQ_EOI);
	}

	outportb(PORT_IRQ_CRT1, IRQ_EOI);
}

