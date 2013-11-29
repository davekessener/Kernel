#include "system.h"
#include "assert.h"
#include "gdt.h"
#include "idt.h"
#include "isrs.h"
#include "irq.h"
#include "vga.h"
#include "timer.h"
#include "keyboard.h"
#include "paging.h"
#include "kmalloc.h"
#include "fs.h"
#include "multiboot.h"
#include "initrd.h"

void printfs();

void main(multiboot_t *mboot_ptr)
{
	gdt_install();
	idt_install();
	isrs_install();
	irq_install();
	timer_install();
	keyboard_install();

	ASSERT(mboot_ptr->mods_count > 0);

	uint32_t initrd_location = *((uint32_t *) mboot_ptr->mods_addr);
	uint32_t static_heap_address = *((uint32_t *)(mboot_ptr->mods_addr + sizeof(uint32_t)));

	vga_init();
	kmalloc_init(static_heap_address);
	paging_init();
	fs_root = initrd_init(initrd_location);
	
	int_enable();
	
	vga_puts("Hello World!\n");
	vga_printf("Loaded %d modules.\n", mboot_ptr->mods_count);

	printfs();

	for(;;);
}

void printfs(void)
{
	int i = 0;
	dirent_t *node = NULL;

	while((node = readdir_fs(fs_root, i)) != NULL)
	{
		fs_node_t *fsnode = finddir_fs(fs_root, node->name);
		vga_printf("Found file '%s' ", fsnode->name);

		if(fsnode->flags & FS_F_DIRECTORY)
		{
			vga_puts("(directory)\n");
		}
		else
		{
			char buf[1024];
			uint32_t l = read_fs(fsnode, 0, 1024, (uint8_t *) buf);
			buf[l] = '\0';
			vga_printf("(file):\n\t\"%s\"\n", l > 0 ? buf : "<EMPTY>");
		}

		i++;
	}
}

