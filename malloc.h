#ifndef __MALLOC_H
#define __MALLOC_H

#include "system.h"
#include "oarray.h"
#include "assert.h"
#include "kmalloc.h"
#include "paging.h"

#define KHEAP_START 0xc0000000
#define KHEAP_INITIAL_SIZE 0x100000

typedef struct __heap
{
	oarray_t index;
	uint32_t start_address, end_address, max_address;
	uint8_t supervisor, readonly;
} heap_t;

#ifdef MALLOC_C
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MIN_SIZE 0x70000
#define HEAP_MAGIC_H 0x327B23C6
#define HEAP_MAGIC_F 0x6B8B4567

typedef struct __heap_header
{
	uint32_t magic;
	uint8_t hole;
	uint32_t size;
} header_t;

typedef struct __heap_fooder
{
	uint32_t magic;
	header_t *header;
} fooder_t;

heap_t *kheap = NULL;
#else
extern heap_t *kheap;
#endif

heap_t *create_heap_at(heap_t *, uint32_t, uint32_t, uint32_t, uint8_t, uint8_t);
void *alloc(uint32_t, uint8_t, heap_t *);
void free(void *, heap_t *);

#endif


