#include "paging.h"

uint32_t *frames;
uint32_t nframes;
page_dir_t *kernel_dir;
page_dir_t *current_dir;

extern volatile uint32_t heap_ptr;

#define IND(v) ((v)>>5)
#define OFF(v) ((v)&0x1f)

void allocate_frame(page_t *, int, int);
void set_frame(uint32_t);
void reset_frame(uint32_t);
int test_frame(uint32_t);
uint32_t find_frame();

void paging_init(void)
{
	int i;
	uint32_t mem_end = 0x1000000;

	nframes = mem_end >> 12;
	frames = kmalloc(IND(nframes));
	memset(frames, 0, IND(nframes));

	kernel_dir = kmalloc_a(sizeof(page_dir_t));
	memset(kernel_dir, 0, sizeof(page_dir_t));
	current_dir = kernel_dir;

	heap_t *tmp_heap = kmalloc(sizeof(heap_t));

	for(i = KHEAP_START ; i < KHEAP_START + KHEAP_INITIAL_SIZE ; i += 0x1000)
	{
		paging_getPage(i, 1, kernel_dir);
	}

	for(i = 0 ; i < heap_ptr ; i += 0x1000)
	{
		allocate_frame(paging_getPage(i, 1, kernel_dir), 0, 0);
	}

	for(i = KHEAP_START ; i < KHEAP_START + KHEAP_INITIAL_SIZE ; i += 0x1000)
	{
		allocate_frame(paging_getPage(i, 1, kernel_dir), 0, 0);
	}

	isrs_installInterruptHandler(14, paging_pageFaultHandler);
	paging_switchPageDir(kernel_dir);
	
	kheap = create_heap_at(tmp_heap, KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xcffff000, 0, 0);
}

void paging_switchPageDir(page_dir_t *new)
{
	uint32_t cr0;
	current_dir = new;
    __asm__ __volatile__("mov %0, %%cr3":: "r"(&new->tPhysical));
	__asm__ __volatile__("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; // Enable paging!
	__asm__ __volatile__("mov %0, %%cr0":: "r"(cr0));
}

page_t *paging_getPage(uint32_t addr, int create, page_dir_t *dir)
{
	addr >>= 12;

	uint32_t idx = addr >> 10;
	if(dir->tables[idx])
	{
		return &dir->tables[idx]->pages[addr & 0x3ff];
	}
	else if(create)
	{
		uint32_t tmp;
		dir->tables[idx] = kmalloc_ap(sizeof(page_table_t), &tmp);
		memset(dir->tables[idx], 0, 0x1000);
		dir->tPhysical[idx] = tmp | 7; // present | rw | user

		return &dir->tables[idx]->pages[addr & 0x3ff];
	}
	else
	{
		return NULL;
	}
}

void paging_pageFaultHandler(sys_regs_t *r)
{
	uint32_t fault_addr;
	__asm__ __volatile__("mov %%cr2, %0" : "=r" (fault_addr));

	int present = !(r->err_code & 1);
	int rw = r->err_code & 2;
	int us = r->err_code & 4;
	int res = r->err_code & 8;
	int id = r->err_code & 16;

	vga_puts("PAGE FAULT(\" ");
	if(present) vga_puts("present ");
	if(rw)      vga_puts("read-only ");
	if(us)      vga_puts("user_mode ");
	if(res)     vga_puts("reserved ");
	if(id)      vga_puts("ins-fetch ");
	vga_printf("\") @0x%08x\n", fault_addr);

	PANIC("Page fault");
}

// # --------------------------------------------------------------------------

void allocate_frame(page_t *page, int is_kernel, int is_writable)
{
	if(page->frame != 0)
	{
		return;
	}
	else
	{
		uint32_t idx = find_frame();
		if(idx == (uint32_t) -1)
		{
			PANIC("NO FREE FRAMES!");
		}

		set_frame(idx << 12);
		page->present = 1;
		page->rw      = is_writable ? 1 : 0;
		page->user    = is_kernel   ? 0 : 1;
		page->frame   = idx;
	}
}

void free_frame(page_t *page)
{
	uint32_t f = page->frame;

	if(f == 0)
	{
		return;
	}
	else
	{
		reset_frame(f);
		page->frame = 0;
	}
}

// # ==========================================================================

void set_frame(uint32_t addr)
{
	uint32_t f = addr >> 12; // addr / 0x1000 since each frame is 4kb (0x1000 bytes) large
	uint32_t i = IND(f); // f / 32 since 32 flags fit in one uint32
	uint32_t o = OFF(f); // offset within a flag-int

	frames[i] |= (1 << o);
}

void reset_frame(uint32_t addr)
{
	uint32_t f = addr >> 12; // addr / 0x1000 since each frame is 4kb (0x1000 bytes) large
	uint32_t i = IND(f); // f / 32 since 32 flags fit in one uint32
	uint32_t o = OFF(f); // offset within a flag-int

	frames[i] &= ~(1 << o);
}

int test_frame(uint32_t addr)
{
	uint32_t f = addr >> 12; // addr / 0x1000 since each frame is 4kb (0x1000 bytes) large
	uint32_t i = IND(f); // f / 32 since 32 flags fit in one uint32
	uint32_t o = OFF(f); // offset within a flag-int

	return (frames[i] & (1 << o)) >> o;
}

uint32_t find_frame(void)
{
	int i, j;
	for(i = 0 ; i < IND(nframes) ; i++)
	{
		if(frames[i] == 0xffffffff) continue;

		for(j = 0 ; j < 32 ; j++)
		{
			if(!(frames[i] & (1 << j)))
			{
				return (i << 5 | j); // i * 32 + j
			}
		}
	}

//	PANIC("NO FREE PAGE LEFT!");

	return (uint32_t) -1;
}

