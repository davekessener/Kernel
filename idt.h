#ifndef __IDT_H
#define __IDT_H

#include "system.h"

void idt_setGate(uint8_t, uint64_t, uint16_t, uint8_t);
void idt_install();

struct idt_entry
{
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

#endif

