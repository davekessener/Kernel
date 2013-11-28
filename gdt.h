#ifndef __GDT_H
#define __GDT_H

#include "system.h"

void gdt_setGate(uint32_t, uint64_t, uint64_t, uint8_t, uint8_t);
void gdt_install();

struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

#endif

