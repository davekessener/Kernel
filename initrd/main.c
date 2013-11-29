#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "initrd.h"

#define BUF_SIZE 1024

size_t filelen(const char *);

int main(int argc, char *argv[])
{
	initrd_t rd;
	char *buf, *s;
	size_t l, buflen = sizeof(uint32_t);
	char *tmpbuf = malloc(BUF_SIZE);
	FILE *tf = tmpfile();
	int i, j;

	rd.fc = 0;
	rd.headers = NULL;

	for(i = 1 ; i < argc ; i++)
	{
		l = filelen(argv[i]);

		if(!l) continue;

		rd.headers = realloc(rd.headers, ++rd.fc * sizeof(initrd_header_t));
		rd.headers[rd.fc - 1].magic = INITRD_MAGIC;
		strncpy(rd.headers[rd.fc - 1].name, argv[i], 128);
		rd.headers[rd.fc - 1].offset = 0;
		rd.headers[rd.fc - 1].length = (uint32_t) l;

		buflen += sizeof(initrd_header_t) + strlen(argv[i]) + sizeof(uint32_t);
	}

	buf = malloc(buflen);
	s = write_initrd(buf, &rd);
	l = s - buf;

	for(i = 0, j ; i < rd.fc ; i++)
	{
		initrd_header_t *h = &rd.headers[i];
		FILE *f = fopen(h->name, "rb");
		j = h->length;
		
		*rd.symtable.offsets[i] = l;

		while(j >= BUF_SIZE)
		{
			fread(tmpbuf, 1, BUF_SIZE, f);
			fwrite(tmpbuf, 1, BUF_SIZE, tf);
			j -= BUF_SIZE;
		}
		if(j > 0)
		{
			fread(tmpbuf, 1, j, f);
			fwrite(tmpbuf, 1, j, tf);
		}

		l += h->length;

		fclose(f);
	}

	fwrite(buf, 1, s - buf, stdout);

	l -= s - buf;
	fseek(tf, 0, SEEK_SET);
	while(l >= BUF_SIZE)
	{
		fread(tmpbuf, 1, BUF_SIZE, tf);
		fwrite(tmpbuf, 1, BUF_SIZE, stdout);
		l -= BUF_SIZE;
	}
	if(l > 0)
	{
		fread(tmpbuf, 1, l, tf);
		fwrite(tmpbuf, 1, l, stdout);
	}

	free(buf);
	free(tmpbuf);

	return EXIT_SUCCESS;
}

size_t filelen(const char *fn)
{
	size_t l;
	FILE *f;

	assert(fn&&*fn);

	f = fopen(fn, "rb");
	assert(f);

	fseek(f, 0, SEEK_END);
	l = ftell(f);
	fclose(f);

	return l;
}

