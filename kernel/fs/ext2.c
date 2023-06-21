#include <fs/ext2.h>

ext2_superblock_t* initrd_superblock;
ext2_inodetable_t* initrd_root_node;
ext2_bgdescriptor_t* initrd_root_block;
ext2_inodetable_t* initrd_inode_table;
void* initrd_start;

fs_node_t* initrd_root;

void open_initrd(fs_node_t* node, uint32_t flags);
void close_initrd(fs_node_t* node);
uint32_t read_initrd(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buffer);
uint32_t write_initrd(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buffer);
struct dirent* readdir_initrd(fs_node_t* node, uint32_t index);
fs_node_t* finddir_initrd(fs_node_t* node, char* name);

/* utilities for dealing with fs blocks and inodes */
ext2_inodetable_t* ext2_get_inode(uint32_t inode) {
	return (ext2_inodetable_t*) ((uintptr_t) initrd_inode_table + initrd_superblock->inode_size * (inode - 1));
}

void* ext2_get_block(uint32_t block) {
    return (void*) ((uintptr_t) initrd_start + (1024 << initrd_superblock->log_block_size) * block);
}

void* ext2_get_inode_block(ext2_inodetable_t* inode, uint32_t block) {
	if (block < 12) {
		return ext2_get_block(inode->block[block]);
	} else if (block < 12 + (1024 << initrd_superblock->log_block_size) / sizeof(uint32_t)) {
		return ext2_get_block(*(uint32_t*) ((uintptr_t) ext2_get_block(inode->block[12]) + (block - 12) * sizeof(uint32_t)));
	} else if (block < 12 + 256 + 256 * 256) {
		uint32_t a = block - 12;
		uint32_t b = a - 256;
		uint32_t c = b / 256;
		uint32_t d = b - c * 256;
		void* c_block = ext2_get_block(*(uint32_t*) ((uintptr_t) ext2_get_block(inode->block[13]) + (c) * sizeof(uint32_t)));
		return ext2_get_block(*(uint32_t*) ((uintptr_t) c_block + (d) * sizeof(uint32_t)));
	}
	return NULL;
}

ext2_dir_t* ext2_get_direntry(ext2_inodetable_t* inode, uint32_t index) {
	void* block = (void*) ext2_get_inode_block(inode,0);
	uint32_t dir_offset, dir_index = 0;

	while (dir_offset < inode->size) {
		ext2_dir_t* d_ent = (ext2_dir_t*) ((uintptr_t) block + dir_offset);
		if (dir_index == index)
			return d_ent;
		dir_offset += d_ent->rec_len;
		dir_index++;
	}
	return NULL;
}

uint32_t initrd_node_from_file(ext2_inodetable_t* inode, ext2_dir_t* direntry, fs_node_t* fnode) {
	if (!fnode) return 0;

	/* Information from the direntry */
	fnode->inode = direntry->inode;
	memcpy(&fnode->name, &direntry->name, direntry->name_len);
	fnode->name[direntry->name_len] = '\0';

	/* Information from the inode */
	fnode->length = inode->size;

	/* File Flags */
	fnode->flags = 0;
	if ((inode->mode & EXT2_S_IFREG) == EXT2_S_IFREG)
		fnode->flags |= FS_FILE;
	if ((inode->mode & EXT2_S_IFDIR) == EXT2_S_IFDIR)
		fnode->flags |= FS_DIRECTORY;
	if ((inode->mode & EXT2_S_IFBLK) == EXT2_S_IFBLK)
		fnode->flags |= FS_BLOCKDEVICE;
	if ((inode->mode & EXT2_S_IFCHR) == EXT2_S_IFCHR)
		fnode->flags |= FS_CHARDEVICE;
	if ((inode->mode & EXT2_S_IFIFO) == EXT2_S_IFIFO)
		fnode->flags |= FS_PIPE;
	if ((inode->mode & EXT2_S_IFLNK) == EXT2_S_IFLNK)
		fnode->flags |= FS_SYMLINK;

	fnode->read    = read_initrd;
	fnode->write   = write_initrd;
	fnode->open    = open_initrd;
	fnode->close   = close_initrd;
	fnode->readdir = readdir_initrd;
	fnode->finddir = finddir_initrd;
	return 1;
}

uint32_t initrd_node_root(ext2_inodetable_t* inode, fs_node_t* fnode) {
	if (!fnode) return 0;

	/* Information from the direntry */
	fnode->inode = 2;
	fnode->name[0] = '/';
	fnode->name[1] = '\0';

	/* Information from the inode */
	fnode->length = inode->size;

	/* File Flags */
	fnode->flags = 0;
	if ((inode->mode & EXT2_S_IFREG) == EXT2_S_IFREG)
		fnode->flags |= FS_FILE;
	if ((inode->mode & EXT2_S_IFDIR) == EXT2_S_IFDIR)
		fnode->flags |= FS_DIRECTORY;
	if ((inode->mode & EXT2_S_IFBLK) == EXT2_S_IFBLK)
		fnode->flags |= FS_BLOCKDEVICE;
	if ((inode->mode & EXT2_S_IFCHR) == EXT2_S_IFCHR)
		fnode->flags |= FS_CHARDEVICE;
	if ((inode->mode & EXT2_S_IFIFO) == EXT2_S_IFIFO)
		fnode->flags |= FS_PIPE;
	if ((inode->mode & EXT2_S_IFLNK) == EXT2_S_IFLNK)
		fnode->flags |= FS_SYMLINK;

	fnode->read    = read_initrd;
	fnode->write   = write_initrd;
	fnode->open    = open_initrd;
	fnode->close   = close_initrd;
	fnode->readdir = readdir_initrd;
	fnode->finddir = finddir_initrd;
	return 1;
}

uint32_t initrd_node_from_dirent(ext2_inodetable_t* inode, struct dirent* direntry, fs_node_t* fnode) {
	if (!fnode) return 0;

	fnode->inode = direntry->inode;
	memcpy(&fnode->name, &direntry->name, strlen(direntry->name));
	fnode->name[strlen(direntry->name)] = '\0';

	/* Information from the inode */
	fnode->length = inode->size;

	/* File Flags */
	fnode->flags = 0;
	if ((inode->mode & EXT2_S_IFREG) == EXT2_S_IFREG)
		fnode->flags |= FS_FILE;
	if ((inode->mode & EXT2_S_IFDIR) == EXT2_S_IFDIR) 
		fnode->flags |= FS_DIRECTORY;
	if ((inode->mode & EXT2_S_IFBLK) == EXT2_S_IFBLK)
		fnode->flags |= FS_BLOCKDEVICE;
	if ((inode->mode & EXT2_S_IFCHR) == EXT2_S_IFCHR)
		fnode->flags |= FS_CHARDEVICE;
	if ((inode->mode & EXT2_S_IFIFO) == EXT2_S_IFIFO)
		fnode->flags |= FS_PIPE;
	if ((inode->mode & EXT2_S_IFLNK) == EXT2_S_IFLNK)
		fnode->flags |= FS_SYMLINK;

	fnode->read    = read_initrd;
	fnode->write   = write_initrd;
	fnode->open    = open_initrd;
	fnode->close   = close_initrd;
	fnode->readdir = readdir_initrd;
	fnode->finddir = finddir_initrd;
	return 1;
}

/* common filesystem operations for VFS */
void open_initrd(fs_node_t* node, uint32_t flags) {
    (void) node;
    (void) flags;
}

void close_initrd(fs_node_t* node) {
    (void) node;
}

uint32_t read_initrd(fs_node_t* node, uint32_t offset, size_t size, uint8_t *buffer) {
	ext2_inodetable_t* inode = ext2_get_inode(node->inode);

	uint32_t end;
	if (offset + size > inode->size) {
		end = inode->size;
	} else {
		end = offset + size;
	}

	uint32_t block_size  = 1024 << initrd_superblock->log_block_size;
	uint32_t start_block = offset / block_size;
	uint32_t end_block   = end / block_size;
	uint32_t end_size    = end % block_size;
	uint32_t size_to_read = end - offset;

	if (start_block == end_block) {
		memcpy(buffer, (void*) ((uintptr_t) ext2_get_inode_block(inode, start_block) + offset % block_size), size_to_read);
		return size_to_read;
	} else {
		uint32_t block_offset = start_block;
		uint32_t blocks_read = 0;

		for (block_offset = start_block; block_offset < end_block; ++block_offset) {
			if (block_offset == start_block) 
				memcpy(buffer, (void*) ((uintptr_t) ext2_get_inode_block(inode, block_offset) + (offset % block_size)), (block_size - (offset % block_size)));
			else
				memcpy(buffer + block_size * blocks_read - (offset % block_size), ext2_get_inode_block(inode, block_offset), block_size);
			blocks_read++;
		}

		memcpy(buffer + block_size * blocks_read - (offset % block_size), ext2_get_inode_block(inode, end_block), end_size);
	}
	return size_to_read;
}

uint32_t write_initrd(fs_node_t* node, uint32_t offset, size_t size, uint8_t *buffer) {
	return 0;
}

struct dirent* readdir_initrd(fs_node_t* node, uint32_t index) {
	ext2_inodetable_t* inode = ext2_get_inode(node->inode);

	if(!(inode->mode & EXT2_S_IFDIR))
        return NULL;

	ext2_dir_t* direntry = ext2_get_direntry(inode, index);

	if (!direntry)
        return NULL;

	struct dirent * dirent = malloc(sizeof(struct dirent));
	memcpy(&dirent->name, &direntry->name, direntry->name_len);
	dirent->name[direntry->name_len] = '\0';
	dirent->inode = direntry->inode;

	return dirent;
}

fs_node_t* finddir_initrd(fs_node_t* node, char* name) {
	ext2_inodetable_t* inode = ext2_get_inode(node->inode);

	if(!(inode->mode & EXT2_S_IFDIR))
        return NULL;

	void* block;
	ext2_dir_t* direntry = NULL;
	block = (void*) ext2_get_inode_block(inode, 0);
	uint32_t dir_offset;
	dir_offset = 0;

	while(dir_offset < inode->size) {
		ext2_dir_t* d_ent = (ext2_dir_t*) ((uintptr_t)block + dir_offset);
		char* dname = kmalloc(sizeof(char) * (d_ent->name_len + 1));
		memcpy(dname, &d_ent->name, d_ent->name_len);
		dname[d_ent->name_len] = '\0';

		if (!strcmp(dname, name)) {
			kfree(dname);
			direntry = d_ent;
			break;
		}
		kfree(dname);
		dir_offset += d_ent->rec_len;
	}

	if (!direntry)
		return NULL;

	fs_node_t* outnode = kmalloc(sizeof(fs_node_t));
	initrd_node_from_file(ext2_get_inode(direntry->inode), direntry, outnode);
	return outnode;
}

/* mount our ramdisk and connect it to the vfs root node */
void ext2_ramdisk_mount(uint32_t mem_head) {
	initrd_start = (void*) mem_head;
	initrd_superblock = (ext2_superblock_t*) ((uintptr_t) initrd_start + 1024);

    if (initrd_superblock->magic != EXT2_SUPER_MAGIC) {
        klog(LOG_ERR, "Valid EXT2 superblock not found\n");
        return;
    }

	if (initrd_superblock->inode_size == 0)
		initrd_superblock->inode_size = 128;

	initrd_root_block = (ext2_bgdescriptor_t*) ((uintptr_t) initrd_start + 1024 + 1024);
	initrd_inode_table = (ext2_inodetable_t*) ((uintptr_t) initrd_start + (1024 << initrd_superblock->log_block_size) * initrd_root_block->inode_table);
	ext2_inodetable_t* root_inode = ext2_get_inode(2);

	initrd_root = (fs_node_t*) kmalloc(sizeof(fs_node_t));
	if (!initrd_node_root(root_inode, initrd_root)) {
        klog(LOG_ERR, "Could not create EXT2 root node\n");
        return;
    }

	fs_root = initrd_root;

	klog(LOG_OK, "Mounted EXT2 ramdisk, root VFS node is 0x%x\n", mem_head, initrd_root);
}
