#include "gdt.h"

#define GDT_COUNT 5

struct gdt_entry gdt[GDT_COUNT];
struct gdt_ptr gp;

extern void gdt_flush();

void gdt_setGate(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran)
{
	gdt[i].base_low    =  base          & 0xffff;
	gdt[i].base_middle = (base   >> 16) & 0x00ff;
	gdt[i].base_high   = (base   >> 24) & 0x00ff;

	gdt[i].limit_low   =   limit        & 0xffff;
	gdt[i].granularity = ((limit >> 16) & 0x000f) | 
						  (gran         & 0x00f0);
	gdt[i].access      = access;
}

void gdt_install(void)
{
	gp.limit = (sizeof(struct gdt_entry) * GDT_COUNT) - 1;
	gp.base = (uint32_t) &gdt;

	gdt_setGate(0, 0, 0, 0, 0); // NULL descriptor
	gdt_setGate(1, 0, 0xffffffff, 0x9a, 0xcf); // CS
	gdt_setGate(2, 0, 0xffffffff, 0x92, 0xcf); // DS
	gdt_setGate(3, 0, 0xffffffff, 0xfa, 0xcf); // user CS
	gdt_setGate(4, 0, 0xffffffff, 0xf2, 0xcf); // user DS

	gdt_flush();
}

