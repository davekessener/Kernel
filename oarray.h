#ifndef __OARRAY_H
#define __OARRAY_H

#include "system.h"
#include "assert.h"

typedef void *type_t;
typedef int8_t (*lt_p_t)(type_t, type_t);

typedef struct __ordered_array
{
	type_t *array;
	uint32_t size;
	uint32_t max_size;
	lt_p_t cmp;
} oarray_t;

#include "kmalloc.h"

int8_t std_lt_p(type_t, type_t);

void OARRAY_init(oarray_t*, uint32_t, lt_p_t);
void OARRAY_initAt(oarray_t*, void *, uint32_t, lt_p_t);
void OARRAY_insert(oarray_t*, type_t);
type_t OARRAY_lookup(oarray_t*, uint32_t);
void OARRAY_remove(oarray_t*, uint32_t);
void OARRAY_dispose(oarray_t*);

#endif

