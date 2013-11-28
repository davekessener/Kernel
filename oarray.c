#include "oarray.h"

int8_t std_lt_p(type_t a, type_t b)
{
	return a < b;
}

void OARRAY_init(oarray_t *this, uint32_t max_size, lt_p_t ltfn)
{
	this->array = kmalloc(max_size * sizeof(type_t));
	memset(this->array, 0, max_size * sizeof(type_t));
	this->size = 0;
	this->max_size = max_size;
	this->cmp = ltfn;
}

void OARRAY_initAt(oarray_t *this, void *addr, uint32_t max_size, lt_p_t ltfn)
{
	this->array = addr;
	memset(this->array, 0, max_size * sizeof(type_t));
	this->size = 0;
	this->max_size = max_size;
	this->cmp = ltfn;
}

void OARRAY_insert(oarray_t *this, type_t item)
{
	int i = 0;

	ASSERT(this->cmp);
	ASSERT(this->size<this->max_size);

	while(i < this->size && this->cmp(this->array[i], item)) i++;

	if(i == this->size)
	{
		this->array[this->size++] = item;
	}
	else
	{
		type_t tmp = this->array[i];
		this->array[i] = item;

		while(i < this->size)
		{
			type_t t = this->array[++i];
			this->array[i] = tmp;
			tmp = t;
		}

		this->size++;
	}
}

type_t OARRAY_lookup(oarray_t *this, uint32_t i)
{
	ASSERT(i<this->size);

	return this->array[i];
}

void OARRAY_remove(oarray_t *this, uint32_t i)
{
	while(i < this->size)
	{
		this->array[i] = this->array[i + 1];
		i++;
	}

	this->size--;
}

void OARRAY_dispose(oarray_t *this)
{
	kfree(this->array);
}

