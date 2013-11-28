#include "idt.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void idt_load();

void idt_setGate(uint8_t i, uint64_t base, uint16_t sel, uint8_t flags)
{
	idt[i].base_lo =  base        & 0xffff;
	idt[i].base_hi = (base >> 16) & 0xffff;

	idt[i].sel     = sel;
	idt[i].always0 = 0;
	idt[i].flags   = flags;
}

void idt_install(void)
{
	idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
	idtp.base = (uint32_t) &idt;

	memset(&idt, 0, sizeof(struct idt_entry) * 256);

	idt_load();
}

