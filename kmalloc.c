#include "kmalloc.h"

extern uint32_t _kend;
uint32_t heap_ptr = (uint32_t) &_kend;

void kmalloc_init(uint32_t saddr)
{
	ASSERT(saddr>=heap_ptr);
	heap_ptr = saddr;
#ifdef DEBUG
	vga_printf("static heap starts @0x%08x!\n", heap_ptr);
#endif
}

void *kmalloc_int(uint32_t sz, int align, uint32_t *phys)
{
	uint32_t tmp;

	if(align == 1 && (heap_ptr & 0x0fff))
	{
		heap_ptr &= 0xfffff000;
		heap_ptr += 0x00001000;
	}

	tmp = heap_ptr;
	heap_ptr += sz;

	if(phys)
	{
		*phys = tmp;
	}

#ifdef DEBUG
	vga_printf("static  kmalloc @0x%08x over % 5d(0x%04x) bytes.\n", tmp, sz, sz);
#endif

	return (void *) tmp;
}

void *kmalloc_a(uint32_t sz)
{
	return kmalloc_int(sz, 1, NULL);
}

void *kmalloc_p(uint32_t sz, uint32_t *phys)
{
	return kmalloc_int(sz, 0, phys);
}

void *kmalloc_ap(uint32_t sz, uint32_t *phys)
{
	return kmalloc_int(sz, 1, phys);
}

void *kmalloc(uint32_t sz)
{
	void *tmp = kheap ? alloc(sz, 0, kheap) : kmalloc_int(sz, 0, NULL);

#ifdef DEBUG
	if(kheap)
	{
		vga_printf("dynamic kmalloc @0x%08x over % 5d(0x%04x) bytes.\n", (uint32_t)tmp, sz, sz);
	}
#endif

	return tmp;
}

void kfree(void *p)
{
	if(kheap)
	{
		free(p, kheap);
	}
}

