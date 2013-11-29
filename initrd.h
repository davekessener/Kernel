#define KERNEL
#include "initrd/initrd.h"
#undef KERNEL

#ifndef __K_INITRD_H
#define __K_INITRD_H

#include "system.h"
#include "fs.h"
#include "string.h"

fs_node_t *initrd_init(uint32_t);

#endif

