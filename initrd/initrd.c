#include "initrd.h"

char *write_initrd(char *buf, initrd_t *rd)
{
	int i;

	*((uint32_t *) buf) = rd->fc;
	buf += sizeof(uint32_t);

	for(i = 0 ; i < rd->fc ; i++)
	{
		rd->symtable.offsets[i] = buf + sizeof(uint32_t);
		buf = write_initrd_header(buf, rd->headers + i);
	}

	return buf;
}

char *write_initrd_header(char *b, initrd_header_t *h)
{
	uint32_t *buf = (uint32_t *) b;

	assert(h->magic==INITRD_MAGIC);

	*buf++ = h->magic;
	*buf++ = h->offset;
	*buf++ = h->length;

	b = (char *) buf;
	memcpy(b, h->name, 128);
	b += 128;

	return b;
}

