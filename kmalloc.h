#ifndef __KMALLOC_H
#define __KMALLOC_H

#include "system.h"
#include "malloc.h"

void kmalloc_init(uint32_t);
void *kmalloc_a(uint32_t);
void *kmalloc_p(uint32_t, uint32_t *);
void *kmalloc_ap(uint32_t, uint32_t *);
void *kmalloc(uint32_t);
void kfree(void *);

#endif

