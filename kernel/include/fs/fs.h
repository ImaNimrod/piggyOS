#ifndef _KERNEL_FS_H
#define _KERNEL_FS_H

#include <display.h>
#include <dsa/tree.h>
#include <memory/kheap.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/spinlock.h>

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_FIFO        0x07

struct fs_node;

typedef void (*open_type_t) (struct fs_node*, uint32_t flags);
typedef void (*close_type_t) (struct fs_node*);
typedef size_t (*read_type_t) (struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef size_t (*write_type_t) (struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef struct dirent *(*readdir_type_t) (struct fs_node*, uint32_t);
typedef struct fs_node *(*finddir_type_t) (struct fs_node*, char* name);
typedef int (*ioctl_type_t) (struct fs_node*, uint32_t request, void* argp);

typedef struct fs_node {
    /* basic file information */
    char name[128]; 
    uint32_t flags;
    uint32_t inode;
    uint32_t length;

    /* time of access and time of creation */
    uint32_t atime;
    uint32_t ctime;

    /* these are func ptrs that outline the operations we can do on files */
    open_type_t open;
    close_type_t close;
    read_type_t read;   
    write_type_t write;
    readdir_type_t readdir; 
    finddir_type_t finddir; 
    ioctl_type_t ioctl;
} fs_node_t;

struct dirent {
    char name[128];
    uint32_t flags;
    uint32_t inode;
    uint32_t offset;
};

struct vfs_entry {
	char* name;
	fs_node_t* file;
};

extern fs_node_t *fs_root;

/* function declarations */
void open_fs(fs_node_t *node, uint32_t flags);
void close_fs(fs_node_t *node);
size_t read_fs(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf);
size_t write_fs(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf);
struct dirent* readdir_fs(fs_node_t* node, uint32_t index);
fs_node_t* finddir_fs(fs_node_t* node, char* name);
int ioctl_fs(fs_node_t* node, uint32_t request, void* argp);
fs_node_t* clone_fs(fs_node_t* node);

char** path(char* file);
void path_free(char** p);

void vfs_init(void);
fs_node_t* get_fs_root(void);
void fs_register(fs_node_t* node);

fs_node_t* rootfs_init(void);

#endif 
