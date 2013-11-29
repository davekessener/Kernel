#include "fs.h"

fs_node_t *fs_root = NULL;

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	return node->read == NULL ? 0 : node->read(node, offset, size, buffer);
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	return node->write == NULL ? 0 : node->write(node, offset, size, buffer);
}

void open_fs(fs_node_t *node, uint8_t read, uint8_t write)
{
	if(node->open != NULL) node->open(node);
}

void close_fs(fs_node_t *node)
{
	if(node->close != NULL) node->close(node);
}

dirent_t *readdir_fs(fs_node_t *node, uint32_t index)
{
	return ((node->flags & FS_F_DIRECTORY) && node->readdir != NULL) ? node->readdir(node, index) : NULL;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name)
{
	return ((node->flags & FS_F_DIRECTORY) && node->finddir != NULL) ? node->finddir(node, name) : NULL;
}

