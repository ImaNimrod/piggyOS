#include <fs/fs.h>

extern fs_node_t *null_device_create(void);
extern fs_node_t *zero_device_create(void);

void devfs_install(const char *path) {
	map_vfs_directory(path);
    kprintf("devfs initialized at %s\n", path);

    vfs_mount("/dev/null", null_device_create());
    vfs_mount("/dev/zero", zero_device_create());
}
