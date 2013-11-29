#ifndef __INITRD_H
#define __INITRD_H

#ifndef KERNEL
#include <assert.h>
#include <string.h>
#endif
#include <stdint.h>

#define INITRD_MAGIC 0x42a70413

typedef struct __initial_ramdisk_header
{
	uint32_t magic;
	uint32_t offset;
	uint32_t length;
	char name[128];
} initrd_header_t;

#ifndef KERNEL
typedef struct __initial_ramdisk_symtable
{
	char *offsets[64];
} initrd_sym_t;
#endif

typedef struct __initial_ramdisk
{
	uint32_t fc;
#ifndef KERNEL
	initrd_header_t *headers;
	initrd_sym_t symtable;
#endif
} initrd_t;

#ifndef KERNEL
char *write_initrd(char *, initrd_t *);
char *write_initrd_header(char *, initrd_header_t *);
#endif

#endif

