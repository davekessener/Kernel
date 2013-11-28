#ifndef __PAGING_H
#define __PAGING_H

#include "system.h"
#include "isrs.h"
#include "kmalloc.h"
#include "assert.h"

typedef struct __page
{
	uint32_t present	:  1; // present in memory
	uint32_t rw			:  1; // 0: read-only, 1: readwrite
	uint32_t user		:  1; // 0: kernel, 1: user
	uint32_t accessed	:  1; // has been accessed since refresh
	uint32_t dirty		:  1; // has been written to "    "
	uint32_t unused		:  7; // unused & reserved
	uint32_t frame		: 20; // frame address
} page_t;

typedef struct __page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct __page_directory
{
	page_table_t *tables[1024];
	uint32_t tPhysical[1024]; // physical address of tables
	uint32_t physicalAddr; // physical address of tPhysical
} page_dir_t;

extern page_dir_t *kernel_dir;

void paging_init();
void paging_switchPageDir(page_dir_t *);
page_t *paging_getPage(uint32_t, int, page_dir_t *);
void free_frame(page_t *);
void allocate_frame(page_t *, int, int);
void paging_pageFaultHandler(sys_regs_t *);

#endif

