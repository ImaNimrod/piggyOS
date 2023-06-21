#ifndef _KERNEL_FS_H
#define _KERNEL_FS_H

#include <display.h>
#include <memory/kheap.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/spinlock.h>
#include <dsa/tree.h>

#define min(l,r) ((l) < (r) ? (l) : (r))

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#define PATH_UP  ".."
#define PATH_DOT "."

/* specified by POSIX */ 
#define MAX_SYMLINK_DEPTH 8
#define MAX_SYMLINK_SIZE 4096

struct fs_node;

typedef void (*open_type_t) (struct fs_node*, uint32_t flags);
typedef void (*close_type_t) (struct fs_node*);
typedef size_t (*read_type_t) (struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef size_t (*write_type_t) (struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef struct dirent *(*readdir_type_t) (struct fs_node*, uint32_t);
typedef struct fs_node *(*finddir_type_t) (struct fs_node*, char* name);
typedef int (*create_type_t) (struct fs_node*, char* name);
typedef int (*mkdir_type_t) (struct fs_node*, char* name);
typedef int (*ioctl_type_t) (struct fs_node*, uint32_t request, void* argp);

typedef struct fs_node {
    /* basic file information */
    char name[128]; 
    void* device; 
    uint32_t flags, inode, length, impl;

    /* these are func ptrs that outline the operations we can do on files */
    open_type_t open;
    close_type_t close;
    read_type_t read;   
    write_type_t write;
    readdir_type_t readdir; 
    finddir_type_t finddir; 
    create_type_t create;
    mkdir_type_t mkdir;
    ioctl_type_t ioctl;
} fs_node_t;

struct vfs_entry {
	char* name;
	fs_node_t* file;
	char* device;
	char* fs_type;
};

struct dirent {
    char name[128];
    uint32_t inode;
};

extern fs_node_t *fs_root;

/* function declarations */
size_t read_fs(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf);
size_t write_fs(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf);
void open_fs(fs_node_t *node, uint32_t flags);
void close_fs(fs_node_t *node);
struct dirent* readdir_fs(fs_node_t* node, uint32_t index);
fs_node_t* finddir_fs(fs_node_t* node, char* name);
int create_file_fs(char* name);
int mkdir_fs(char* name);
int ioctl_fs(fs_node_t* node, uint32_t request, void* argp);

char *canonicalize_path(const char *cwd, const char *input);
fs_node_t *kopen(const char *filename, uint32_t flags);

void vfs_init(void);

void *vfs_mount(const char *path, fs_node_t *local_root);
fs_node_t *get_mount_point(char *path, uint32_t path_depth, char **outpath, uint32_t *outdepth);
int vfs_mount_type(const char *type, const char *arg, const char *mountpoint);

#endif 
