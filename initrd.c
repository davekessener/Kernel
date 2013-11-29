#include "initrd.h"

initrd_t *initrd_;
initrd_header_t *initrd_headers;
fs_node_t *initrd_root;
fs_node_t *initrd_dev;
fs_node_t *root_nodes;
int nroot_nodes;
dirent_t dirent;

static uint32_t initrd_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	initrd_header_t *header = &initrd_headers[node->inode];

	if(offset > header->length)
	{
		return 0;
	}

	if(offset + size > header->length)
	{
		size = header->length - offset;
	}

	memmove(buffer, (uint8_t *) (header->offset + offset), size);

	return size;
}

static dirent_t *initrd_readdir(fs_node_t *node, uint32_t index)
{
	if(node == initrd_root && index == 0)
	{
		strcpy(dirent.name, "dev");
		dirent.ino = 0;
		return &dirent;
	}

	if(index - 1 >= nroot_nodes)
	{
		return NULL;
	}

	strcpy(dirent.name, root_nodes[index - 1].name);
	dirent.ino = root_nodes[index - 1].inode;

	return &dirent;
}

static fs_node_t *initrd_finddir(fs_node_t *node, char *name)
{
	if(node == initrd_root && strcmp(name, "dev") == 0)
	{
		return initrd_dev;
	}

	int i;
	for(i = 0 ; i < nroot_nodes ; i++)
	{
		if(strcmp(name, root_nodes[i].name) == 0)
		{
			return &root_nodes[i];
		}
	}

	return NULL;
}

fs_node_t *initrd_init(uint32_t location)
{
	initrd_= (initrd_t *) location;
	initrd_headers = (initrd_header_t *) (location + sizeof(initrd_t));

	initrd_root = kmalloc(sizeof(fs_node_t));
	strcpy(initrd_root->name, "initrd");
	initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
	initrd_root->flags = FS_F_DIRECTORY;
	initrd_root->read = NULL;
	initrd_root->write = NULL;
	initrd_root->open = NULL;
	initrd_root->close = NULL;
	initrd_root->readdir = &initrd_readdir;
	initrd_root->finddir = &initrd_finddir;
	initrd_root->ptr = NULL;
	initrd_root->impl = 0;

	initrd_dev = kmalloc(sizeof(fs_node_t));
	strcpy(initrd_dev->name, "dev");
	initrd_dev->mask = initrd_dev->uid = initrd_dev->gid = initrd_dev->inode = initrd_dev->length = 0;
	initrd_dev->flags = FS_F_DIRECTORY;
	initrd_dev->read = NULL;
	initrd_dev->write = NULL;
	initrd_dev->open = NULL;
	initrd_dev->close = NULL;
	initrd_dev->readdir = &initrd_readdir;
	initrd_dev->finddir = &initrd_finddir;
	initrd_dev->ptr = NULL;
	initrd_dev->impl = 0;

	root_nodes = kmalloc(sizeof(fs_node_t) * initrd_->fc);
	nroot_nodes = initrd_->fc;

	int i;
	for(i = 0 ; i < initrd_->fc ; i++)
	{
		initrd_headers[i].offset += location;

		strcpy(root_nodes[i].name, initrd_headers[i].name);
		root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
		root_nodes[i].length = initrd_headers[i].length;
		root_nodes[i].inode = i;
		root_nodes[i].flags = FS_F_FILE;
		root_nodes[i].read = &initrd_read;
		root_nodes[i].write = NULL;
		root_nodes[i].readdir = NULL;
		root_nodes[i].finddir = NULL;
		root_nodes[i].open = NULL;
		root_nodes[i].close = NULL;
		root_nodes[i].ptr = NULL;
		root_nodes[i].impl = 0;
	}

	return initrd_root;
}

