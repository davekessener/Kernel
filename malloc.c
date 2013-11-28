#define MALLOC_C
#include "malloc.h"
#undef MALLOC_C

int32_t find_smallest_hole(uint32_t size, uint8_t page_align, heap_t *heap)
{
	int i = 0;

	while(i < heap->index.size)
	{
		header_t *header = OARRAY_lookup(&heap->index, i);
		uint32_t location;

		if(page_align && ((location = ((uint32_t)header) + sizeof(header_t)) & 0xfffff000))
		{
			if(header->size >= size + (0x1000 - (location & 0x0fff))) break; // same as: header->size - offset >= size
		}
		else if(header->size >= size)
		{
			break;
		}

		i++;
	}

	return (i == heap->index.size) ? -1 : i;
}

int8_t heap_lt_cmp(type_t a, type_t b)
{
	return ((header_t *) a)->size < ((header_t *) b)->size;
}

heap_t *create_heap_at(heap_t *heap, uint32_t start_addr, uint32_t end_addr, uint32_t max_addr, uint8_t supervisor, uint8_t readonly)
{
	ASSERT((start_addr&0x0fff)==0);
	ASSERT((end_addr&0x0fff)==0);

	OARRAY_initAt(&heap->index, (void *) start_addr, HEAP_INDEX_SIZE, heap_lt_cmp);

	start_addr += sizeof(type_t) * HEAP_INDEX_SIZE;

	if(start_addr & 0xfffff000)
	{
		start_addr &= 0xfffff000;
		start_addr += 0x1000;
	}

	heap->start_address = start_addr;
	heap->end_address   = end_addr;
	heap->max_address   = max_addr;
	heap->supervisor    = supervisor;
	heap->readonly      = readonly;

	header_t *hole = (header_t *) start_addr;
	hole->size     = end_addr - start_addr;
	hole->magic    = HEAP_MAGIC_H;
	hole->hole     = 1;

	fooder_t *fend = (fooder_t *) (end_addr - sizeof(fooder_t));
	fend->magic    = HEAP_MAGIC_F;
	fend->header   = hole;

	OARRAY_insert(&heap->index, (type_t) hole);

	return heap;
}

void expand(heap_t *heap, uint32_t nsize)
{
	ASSERT(nsize>heap->end_address-heap->start_address);

	if(nsize & 0xfffff000)
	{
		nsize &= 0xfffff000;
		nsize += 0x1000;
	}

	ASSERT(heap->start_address+nsize<=heap->max_address);

	uint32_t i = heap->end_address - heap->start_address;

	while(i < nsize)
	{
		allocate_frame(paging_getPage(heap->start_address + i, 1, kernel_dir), heap->supervisor, heap->readonly);
		i += 0x1000;
	}

	heap->end_address = heap->start_address + nsize;
}

uint32_t contract(heap_t *heap, uint32_t nsize)
{
	ASSERT(nsize<heap->end_address-heap->start_address);

	if(nsize & 0xfffff000)
	{
		nsize &= 0xfffff000;
		nsize += 0x1000;
	}

	if(nsize < HEAP_MIN_SIZE)
	{
		nsize = HEAP_MIN_SIZE;
	}

	uint32_t i = heap->end_address - heap->start_address - 0x1000;
	while(nsize < i)
	{
		free_frame(paging_getPage(heap->start_address + i, 0, kernel_dir));
		i -= 0x1000;
	}

	heap->end_address = heap->start_address + nsize;

	return nsize;
}

void *alloc(uint32_t size, uint8_t page_align, heap_t *heap)
{
	uint32_t new_size = size + sizeof(header_t) + sizeof(fooder_t);
	int i = find_smallest_hole(new_size, page_align, heap);

	if(i == -1)
	{
		uint32_t old_length = heap->end_address - heap->start_address;
		uint32_t old_end_address = heap->end_address;

		expand(heap, old_length + new_size);

		uint32_t new_length = heap->end_address - heap->start_address;

		i = 0;

		uint32_t idx = -1, value = 0;
		while(i < heap->index.size)
		{
			uint32_t tmp = (uint32_t) OARRAY_lookup(&heap->index, i);

			if(tmp > value)
			{
				value = tmp;
				idx = i;
			}

			i++;
		}

		if(idx == -1)
		{
			header_t *header = (header_t *) old_end_address;
			header->magic = HEAP_MAGIC_H;
			header->size = new_length - old_length;
			header->hole = 1;

			fooder_t *fooder = (fooder_t *) (old_end_address + header->size - sizeof(fooder_t));
			fooder->magic = HEAP_MAGIC_F;
			fooder->header = header;

			OARRAY_insert(&heap->index, (void *) header);
		}
		else
		{
			header_t *header = OARRAY_lookup(&heap->index, idx);
			header->size += new_length - old_length;
			fooder_t *fooder = (fooder_t *) ((uint32_t)header + header->size - sizeof(fooder_t));
			fooder->magic = HEAP_MAGIC_F;
			fooder->header = header;
		}

		return alloc(size, page_align, heap);
	}

	header_t *orig_hole_header = (header_t *) OARRAY_lookup(&heap->index, i);
	uint32_t  orig_hole_pos    = (uint32_t) orig_hole_header;
	uint32_t  orig_hole_size   = orig_hole_header->size;

	if(orig_hole_size - new_size < sizeof(header_t) + sizeof(fooder_t))
	{
		size += orig_hole_size - new_size;
		new_size = orig_hole_size;
	}

	if(page_align && orig_hole_pos & 0xfffff000)
	{
		uint32_t new_location = 0x1000 - (orig_hole_pos & 0x0fff) - sizeof(header_t);
		
		header_t *hole_header = (header_t *) orig_hole_pos;
		hole_header->size     = new_location;
		hole_header->magic    = HEAP_MAGIC_H;
		hole_header->hole     = 1;
		
		new_location         += orig_hole_pos;

		fooder_t *hole_fooder = (fooder_t *) (new_location - sizeof(fooder_t));
		hole_fooder->magic    = HEAP_MAGIC_F;
		hole_fooder->header   = hole_header;
		
		orig_hole_pos         = new_location;
		orig_hole_size       -= hole_header->size;
	}
	else
	{
		OARRAY_remove(&heap->index, i);
	}
	
	header_t *block_header = (header_t *) orig_hole_pos;
	block_header->magic = HEAP_MAGIC_H;
	block_header->hole = 0;
	block_header->size = new_size;

	fooder_t *block_fooder = (fooder_t *) (orig_hole_pos + sizeof(header_t) + size);
	block_fooder->magic = HEAP_MAGIC_F;
	block_fooder->header = block_header;

	if(orig_hole_size - new_size > 0)
	{
		header_t *hole_header = (header_t *) (orig_hole_pos + sizeof(header_t) + size + sizeof(fooder_t));
		hole_header->magic = HEAP_MAGIC_H;
		hole_header->hole = 1;
		hole_header->size = orig_hole_size - new_size;

		fooder_t *hole_fooder = (fooder_t *) ((uint32_t)hole_header + orig_hole_size - new_size - sizeof(fooder_t));

		if((uint32_t)hole_fooder < heap->end_address)
		{
			hole_fooder->magic = HEAP_MAGIC_F;
			hole_fooder->header = hole_header;
		}

		OARRAY_insert(&heap->index, (void *) hole_header);
	}

	return (void *) ((uint32_t)block_header + sizeof(header_t));
}

void free(void *p, heap_t *heap)
{
	if(p == NULL) return;

	header_t *header = (header_t *) ((uint32_t)p - sizeof(header_t));
	fooder_t *fooder = (fooder_t *) ((uint32_t)header + header->size - sizeof(fooder_t));

	ASSERT(header->magic==HEAP_MAGIC_H);
	ASSERT(fooder->magic==HEAP_MAGIC_F);

	header->hole = 1;
	uint8_t do_add = 1;

	fooder_t *test_fooder = (fooder_t *) ((uint32_t)header - sizeof(fooder_t));
	if(test_fooder->magic == HEAP_MAGIC_F && test_fooder->header->hole)
	{
		uint32_t cache_size = header->size;
		header = test_fooder->header;
		fooder->header = header;
		header->size += cache_size;
		do_add = 0;
	}

	header_t *test_header = (header_t *) ((uint32_t)fooder + sizeof(fooder_t));
	if(test_header->magic == HEAP_MAGIC_H && test_header->hole)
	{
		header->size += test_header->size;
		test_fooder = (fooder_t *) ((uint32_t)test_header + test_header->size - sizeof(fooder_t));
		fooder = test_fooder;

		uint32_t i = 0;
		while(i < heap->index.size && OARRAY_lookup(&heap->index, i) != (void *)test_header)
		{
			i++;
		}

		ASSERT(i<heap->index.size);

		OARRAY_remove(&heap->index, i);
	}

	if((uint32_t)fooder + sizeof(fooder_t) == heap->end_address)
	{
		uint32_t old_length = heap->end_address - heap->start_address;
		uint32_t new_length = contract(heap, (uint32_t)header - heap->start_address);

		if(header->size - (old_length - new_length) > 0)
		{
			header->size = old_length - new_length;
			fooder = (fooder_t *) ((uint32_t)header + header->size - sizeof(fooder_t));
			fooder->magic = HEAP_MAGIC_F;
			fooder->header = header;
		}
		else
		{
			uint32_t i = 0;
			while(i < heap->index.size && OARRAY_lookup(&heap->index, i) != (void *) header)
			{
				i++;
			}

			if(i < heap->index.size)
			{
				OARRAY_remove(&heap->index, i);
			}
		}
	}

	if(do_add)
	{
		OARRAY_insert(&heap->index, (type_t) header);
	}
}

