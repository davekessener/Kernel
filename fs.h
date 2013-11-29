#ifndef __FS_H
#define __FS_H

#include "system.h"

#define FS_NODE_NAME_MAXLEN 128
#define FS_F_FILE 1
#define FS_F_DIRECTORY 2
#define FS_F_CHARDEVICE 4
#define FS_F_BLOCKDEVICE 8
#define FS_F_PIPE 16
#define FS_F_SYMLINK 32
#define FS_F_MOUNTPOINT 64

struct __filesystem_node;
struct __directory_entry;

typedef uint32_t (*read_fn_t)(struct __filesystem_node *, uint32_t, uint32_t, uint8_t *);
typedef uint32_t (*write_fn_t)(struct __filesystem_node *, uint32_t, uint32_t, uint8_t *);
typedef void (*open_fn_t)(struct __filesystem_node *);
typedef void (*close_fn_t)(struct __filesystem_node *);
typedef struct __directory_entry *(*readdir_fn_t)(struct __filesystem_node *, uint32_t);
typedef struct __filesystem_node *(*finddir_fn_t)(struct __filesystem_node *, char *);

typedef struct __filesystem_node
{
	char name[FS_NODE_NAME_MAXLEN];
	uint32_t mask;
	uint32_t uid;
	uint32_t gid;
	uint32_t flags;
	uint32_t inode;
	uint32_t length;
	uint32_t impl;
	read_fn_t read;
	write_fn_t write;
	open_fn_t open;
	close_fn_t close;
	readdir_fn_t readdir;
	finddir_fn_t finddir;
	struct __filesystem_node *ptr;
} fs_node_t;

typedef struct __directory_entry
{
	char name[FS_NODE_NAME_MAXLEN];
	uint32_t ino;
} dirent_t;

extern fs_node_t *fs_root;

uint32_t read_fs(fs_node_t *, uint32_t, uint32_t, uint8_t *);
uint32_t write_fs(fs_node_t *, uint32_t, uint32_t, uint8_t *);
void open_fs(fs_node_t *, uint8_t, uint8_t);
void close_fs(fs_node_t *);
dirent_t *readdir_fs(fs_node_t *, uint32_t);
fs_node_t *finddir_fs(fs_node_t *, char *);

#endif

