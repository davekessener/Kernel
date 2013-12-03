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

	if(kheap)
	{
		tmp = (uint32_t) alloc(sz, (uint8_t) align, kheap);
		if(phys)
		{
			page_t *page = paging_getPage(tmp, 0, kernel_dir);
			*phys = page->frame * 0x1000 + (tmp & 0xfff);
		}
	}
	else
	{
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
	}

#ifdef DEBUG
	vga_printf("%s kmalloc @0x%08x over % 5d(0x%04x) bytes.\n", kheap ? "dynamic" : "static ", tmp, sz, sz);
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
	return kmalloc_int(sz, 0, NULL);
}

void kfree(void *p)
{
	if(kheap)
	{
		free(p, kheap);
	}
}

