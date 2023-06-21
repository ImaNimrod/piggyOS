#include <fs/ext2.h>

#define BLOCKSIZE		1024
#define SECTORSIZE		512
#define CACHEENTRIES	10240

typedef struct {
	uint32_t block_no;
	uint32_t last_use;
	uint8_t  dirty;
	uint8_t  block[BLOCKSIZE];
} ext2_ramdisk_cache_entry_t;

uint32_t _ramdisk_offset = 0;

ext2_ramdisk_cache_entry_t *ext2_ramdisk_cache  = NULL;
ext2_superblock_t* ext2_ramdisk_superblock      = NULL;
ext2_bgdescriptor_t* ext2_ramdisk_root_block    = NULL;
static fs_node_t* ext2_root_fsnode              = NULL;

/** Prototypes */
uint32_t ext2_ramdisk_node_from_file(ext2_inodetable_t* inode, ext2_dir_t* direntry, fs_node_t* fnode);
ext2_inodetable_t* ext2_ramdisk_inode(uint32_t inode);
ext2_inodetable_t* ext2_ramdisk_alloc_inode(ext2_inodetable_t* parent, uint32_t no, char* name, uint16_t mode, uint32_t* inode_no);
fs_node_t* finddir_ext2_ramdisk(fs_node_t* node, char* name);
void insertdir_ext2_ramdisk(ext2_inodetable_t* p_node, uint32_t no, uint32_t inode, char* name, uint8_t type);
void ext2_ramdisk_write_inode(ext2_inodetable_t* inode, uint32_t index);
size_t write_ext2_ramdisk(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buffer);
size_t read_ext2_ramdisk(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buffer);

uint32_t ext2_ramdisk_inodes_per_group = 0;
uint32_t ext2_ramdisk_bg_descriptors = 0;		// Total number of block groups

#define BGDS ext2_ramdisk_bg_descriptors
#define SB ext2_ramdisk_superblock
#define BGD ext2_ramdisk_root_block
#define RN ext2_root_fsnode
#define DC ext2_ramdisk_cache

#define BLOCKBIT(n)  (bg_buffer[((n) >> 3)] & (1 << (((n) % 8))))
#define BLOCKBYTE(n) (bg_buffer[((n) >> 3)])
#define SETBIT(n)    (1 << (((n) % 8)))

static inline uint32_t btos(uint32_t block) {
	return block * (BLOCKSIZE / SECTORSIZE); 
}

static uint32_t now = 1;
static inline uint32_t ext2_time(void) {
	return now++;
}

static void ramdisk_read_sector(uint32_t lba, uint8_t* buffer) {
	char* src = (char*) (lba * SECTORSIZE + _ramdisk_offset);
	memcpy(buffer, src, SECTORSIZE);
}

static void ramdisk_write_sector(uint32_t lba, uint8_t* buffer) {
	return;
}

static void ext2_flush_dirty(uint32_t ent_no) {
	for (uint32_t i = 0; i < BLOCKSIZE / SECTORSIZE; ++i)
		ramdisk_write_sector(btos(DC[ent_no].block_no) + i, (uint8_t*) ((uint32_t) &DC[ent_no].block + SECTORSIZE * i));

	DC[ent_no].dirty = 0;
}

void ext2_ramdisk_read_block(uint32_t block_no, uint8_t* buf) {
	if (!block_no) return;

	int oldest = -1;
	uint32_t oldest_age = UINT32_MAX;
	for (uint32_t i = 0; i < CACHEENTRIES; ++i) {
		if (DC[i].block_no == block_no) {
			DC[i].last_use = ext2_time();
			memcpy(buf, &DC[i].block, BLOCKSIZE);
			return;
		}

		if (DC[i].last_use < oldest_age) {
			oldest = i;
			oldest_age = DC[i].last_use;
		}
	}

	for (uint32_t i = 0; i < BLOCKSIZE / SECTORSIZE; ++i) 
		ramdisk_read_sector(btos(block_no) + i, (uint8_t*) ((uint32_t) &(DC[oldest].block) + SECTORSIZE * i));

	if (DC[oldest].dirty)
		ext2_flush_dirty(oldest);

	memcpy(buf, &DC[oldest].block, BLOCKSIZE);
	DC[oldest].block_no = block_no;
	DC[oldest].last_use = ext2_time();
	DC[oldest].dirty = 0;
}

void ext2_ramdisk_write_block(uint32_t block_no, uint8_t *buf) {
	if (!block_no) {
		kprintf("[kernel/ext2] block_no = 0?\n");
		kprintf("[kernel/ext2] Investigate the call before this, you have done something terrible!\n");
		return;
	}

	int oldest = -1;
	uint32_t oldest_age = UINT32_MAX;
	for (uint32_t i = 0; i < CACHEENTRIES; ++i) {
		if (DC[i].block_no == block_no) {
			DC[i].last_use = ext2_time();
			DC[i].dirty = 1;
			memcpy(&DC[i].block, buf, BLOCKSIZE);
			return;
		}

		if (DC[i].last_use < oldest_age) {
			oldest = i;
			oldest_age = DC[i].last_use;
		}
	}

	if (DC[oldest].dirty)
		ext2_flush_dirty(oldest);

	memcpy(&DC[oldest].block, buf, BLOCKSIZE);
	DC[oldest].block_no = block_no;
	DC[oldest].last_use = ext2_time();
	DC[oldest].dirty = 1;
}

static void ext2_set_real_block(ext2_inodetable_t* inode, uint32_t block, uint32_t real) {
	if (block < 12) {
		inode->block[block] = real;
		return;
	} else if (block < 12 + BLOCKSIZE / sizeof(uint32_t)) {
		uint8_t* tmp = kmalloc(BLOCKSIZE);
		ext2_ramdisk_read_block(inode->block[12], tmp);

		((uint32_t*) tmp)[block - 12] = real;
		ext2_ramdisk_write_block(inode->block[12], tmp);

		kfree(tmp);
		return;
	} else if (block < 12 + 256 + 256 * 256) {
		uint32_t a = block - 12;
		uint32_t b = a - 256;
		uint32_t c = b / 256;
		uint32_t d = b - c * 256;

		uint8_t* tmp = kmalloc(BLOCKSIZE);
		ext2_ramdisk_read_block(inode->block[13], tmp);

		uint32_t nblock = ((uint32_t*) tmp)[c];
		ext2_ramdisk_read_block(nblock, tmp);
		((uint32_t*) tmp)[d] = real;
		ext2_ramdisk_write_block(nblock, tmp);

		kfree(tmp);
		return;
	} else if (block < 12 + 256 + 256 * 256 + 256 * 256 * 256) {
		uint32_t a = block - 12;
		uint32_t b = a - 256;
		uint32_t c = b - 256 * 256;
		uint32_t d = c / (256 * 256);
		uint32_t e = c - d * 256 * 256;
		uint32_t f = e / 256;
		uint32_t g = e - f * 256;

		uint8_t* tmp = kmalloc(BLOCKSIZE);
		ext2_ramdisk_read_block(inode->block[14], tmp);

		uint32_t nblock = ((uint32_t *)tmp)[d];
		ext2_ramdisk_read_block(nblock, tmp);
		nblock = ((uint32_t *)tmp)[f];
		ext2_ramdisk_read_block(nblock, tmp);
		((uint32_t *)tmp)[g] = nblock;
		ext2_ramdisk_write_block(nblock, tmp);

		kfree(tmp);
		return;
	}

	kpanic("Attempted to set a file block that was too high :(", NULL);
}

static uint32_t ext2_get_real_block(ext2_inodetable_t* inode, uint32_t block) {
	if (block < 12) {
		return inode->block[block];
	} else if (block < 12 + BLOCKSIZE / sizeof(uint32_t)) {
		uint8_t* tmp = kmalloc(BLOCKSIZE);
		ext2_ramdisk_read_block(inode->block[12], tmp);
		uint32_t nblock = ((uint32_t*) tmp) [block - 12];

		kfree(tmp);
		return nblock;
	} else if (block < 12 + 256 + 256 * 256) {
		uint32_t a = block - 12;
		uint32_t b = a - 256;
		uint32_t c = b / 256;
		uint32_t d = b - c * 256;

		uint8_t* tmp = kmalloc(BLOCKSIZE);
		ext2_ramdisk_read_block(inode->block[13], tmp);

		uint32_t nblock = ((uint32_t*) tmp)[c];
		ext2_ramdisk_read_block(nblock, tmp);
		nblock = ((uint32_t*) tmp)[d];

		kfree(tmp);
		return nblock;
	} else if (block < 12 + 256 + 256 * 256 + 256 * 256 * 256) {
		uint32_t a = block - 12;
		uint32_t b = a - 256;
		uint32_t c = b - 256 * 256;
		uint32_t d = c / (256 * 256);
		uint32_t e = c - d * 256 * 256;
		uint32_t f = e / 256;
		uint32_t g = e - f * 256;

		uint8_t* tmp = kmalloc(BLOCKSIZE);
		ext2_ramdisk_read_block(inode->block[14], tmp);
		uint32_t nblock = ((uint32_t*) tmp)[d];

		ext2_ramdisk_read_block(nblock, tmp);
		nblock = ((uint32_t*) tmp)[f];

		ext2_ramdisk_read_block(nblock, tmp);
		nblock = ((uint32_t*) tmp)[g];

		kfree(tmp);
		return nblock;
	}

    kpanic("Attempted to get a file block that was too high :(", NULL);
	return 0;
}

void ext2_ramdisk_inode_alloc_block(ext2_inodetable_t* inode, uint32_t inode_no, uint32_t block) {
	kprintf("Allocating block %d for inode #%d\n", block, inode_no);
	uint32_t block_no = 0, block_offset = 0, group = 0;
	char* bg_buffer = kmalloc(BLOCKSIZE);

	for (uint32_t i = 0; i < BGDS; ++i) {
		if (BGD[i].free_blocks_count > 0) {
			ext2_ramdisk_read_block(BGD[i].block_bitmap, (uint8_t*) bg_buffer);
			while (BLOCKBIT(block_offset))
				++block_offset;

			block_no = block_offset + SB->blocks_per_group * i + 1;
			group = i;
			break;
		}
	}

	if (!block_no) {
		kprintf("[kernel/ext2] No available blocks!\n");
		kfree(bg_buffer);
		return;
	}

	uint8_t b = BLOCKBYTE(block_offset);
	b |= SETBIT(block_offset);
	BLOCKBYTE(block_offset) = b;
	ext2_ramdisk_write_block(BGD[group].block_bitmap, (uint8_t*) bg_buffer);
	kfree(bg_buffer);

	ext2_set_real_block(inode, block, block_no);

	BGD[group].free_blocks_count -= 1;
	ext2_ramdisk_write_block(2, (uint8_t*) BGD);

	inode->blocks++;
	ext2_ramdisk_write_inode(inode, inode_no);
}

uint32_t ext2_ramdisk_inode_read_block(ext2_inodetable_t* inode, uint32_t no, uint32_t block, uint8_t* buf) {
	if (block >= inode->blocks) {
		memset(buf, 0, BLOCKSIZE);
		kprintf("[kernel/ext2] An invalid inode block [%d] was requested [have %d]\n", block, inode->blocks);
		return 0;
	}

	uint32_t real_block = ext2_get_real_block(inode, block);
	ext2_ramdisk_read_block(real_block, buf);
	return real_block;
}

uint32_t ext2_ramdisk_inode_write_block(ext2_inodetable_t* inode, uint32_t inode_no, uint32_t block, uint8_t *buf) {
	while (block >= inode->blocks) {
		ext2_ramdisk_inode_alloc_block(inode, inode_no, inode->blocks);
		if (block != inode->blocks - 1) {
			uint32_t real_block = ext2_get_real_block(inode, inode->blocks - 1);
			uint8_t empty[1024] = {0};

			memset(empty, 0, 1024);
			ext2_ramdisk_write_block(real_block, empty);
		}
	}

	uint32_t real_block = ext2_get_real_block(inode, block);

	kprintf("Virtual block %d maps to real block %d.\n", block, real_block);

	ext2_ramdisk_write_block(real_block, buf);
	return real_block;
}

static uint8_t mode_to_filetype(uint16_t mode) {
	uint16_t ftype = mode & 0xF000;

	switch (ftype) {
		case EXT2_S_IFREG:
			return 1;
		case EXT2_S_IFDIR:
			return 2;
		case EXT2_S_IFCHR:
			return 3;
		case EXT2_S_IFBLK:
			return 4;
		case EXT2_S_IFIFO:
			return 5;
		case EXT2_S_IFSOCK:
			return 6;
		case EXT2_S_IFLNK:
			return 7;
	}

	return 0;
}

ext2_inodetable_t* ext2_ramdisk_alloc_inode(ext2_inodetable_t* parent, uint32_t no, char* name, uint16_t mode, uint32_t* inode_no) {
	if ((parent->mode & EXT2_S_IFDIR) == 0 || name == NULL) {
		kprintf("[kernel/ext2] No name or bad parent.\n");
		return NULL;
	}
	
	ext2_inodetable_t* inode;

	uint32_t node_no = 0, node_offset = 0, group = 0;
	char* bg_buffer = kmalloc(BLOCKSIZE);

	for (uint32_t i = 0; i < BGDS; ++i) {
		if (BGD[i].free_inodes_count > 0) {
			ext2_ramdisk_read_block(BGD[i].inode_bitmap, (uint8_t*) bg_buffer);
			while (BLOCKBIT(node_offset))
				++node_offset;
			node_no = node_offset + ext2_ramdisk_inodes_per_group * i + 1;
			group = i;
			break;
		}
	}

	if (!node_no) {
		kprintf("[kernel/ext2] Failure: No free inodes in block descriptors!\n");
		kfree(bg_buffer);
		return NULL;
	}

	uint8_t b = BLOCKBYTE(node_offset);
	b |= SETBIT(node_offset);
	BLOCKBYTE(node_offset) = b;
	ext2_ramdisk_write_block(BGD[group].inode_bitmap, (uint8_t*) bg_buffer);
	free(bg_buffer);
	BGD[group].free_inodes_count -= 1;
	ext2_ramdisk_write_block(2, (uint8_t*) BGD);
	
	inode = ext2_ramdisk_inode(node_no);
	inode->size = 0;
	inode->blocks = 0;
	inode->mode = mode;
	ext2_ramdisk_write_inode(inode, node_no);
	*inode_no = node_no;

	uint8_t ftype = mode_to_filetype(mode);
	kprintf("[kernel/ext2] Allocated inode, inserting directory entry [%d]...\n", node_no);
	insertdir_ext2_ramdisk(parent, no, node_no, name, ftype);

	return inode;
}

ext2_dir_t* ext2_ramdisk_direntry(ext2_inodetable_t* inode, uint32_t no, uint32_t index) {
	uint8_t* block = kmalloc(BLOCKSIZE);
	uint8_t block_nr = 0;

	ext2_ramdisk_inode_read_block(inode, no, block_nr, block);

	uint32_t dir_offset = 0;
	uint32_t total_offset = 0;
	uint32_t dir_index = 0;
	
	while (total_offset < inode->size && dir_index <= index) {
		ext2_dir_t* d_ent = (ext2_dir_t*) ((uintptr_t) block + dir_offset);
	
		if (dir_index == index) {
			ext2_dir_t* out = kmalloc(d_ent->rec_len);
			memcpy(out, d_ent, d_ent->rec_len);
			kfree(block);
			return out;
		}

		dir_offset += d_ent->rec_len;
		total_offset += d_ent->rec_len;
		dir_index++;

		if (dir_offset >= BLOCKSIZE) {
			block_nr++;
			dir_offset -= BLOCKSIZE;
			ext2_ramdisk_inode_read_block(inode, no, block_nr, block);
		}
	}
	
	kfree(block);
	return NULL;
}

ext2_inodetable_t* ext2_ramdisk_inode(uint32_t inode) {
	uint32_t group = inode / ext2_ramdisk_inodes_per_group;
	if (group > BGDS)
		return NULL;

	uint32_t inode_table_block = BGD[group].inode_table;
	inode -= group * ext2_ramdisk_inodes_per_group;

	uint32_t block_offset		= ((inode - 1) * SB->inode_size) / BLOCKSIZE;
	uint32_t offset_in_block    = (inode - 1) - block_offset * (BLOCKSIZE / SB->inode_size);
	uint8_t* buf                = kmalloc(BLOCKSIZE);
	ext2_inodetable_t* inodet   = kmalloc(BLOCKSIZE);
 
	ext2_ramdisk_read_block(inode_table_block + block_offset, buf);
	ext2_inodetable_t* inodes = (ext2_inodetable_t*) buf;
	memcpy(inodet, &inodes[offset_in_block], sizeof(ext2_inodetable_t));

	kfree(buf);
	return inodet;
}

void ext2_ramdisk_write_inode(ext2_inodetable_t* inode, uint32_t index) {
	uint32_t group = index / ext2_ramdisk_inodes_per_group;
	if (group > BGDS)
		return;
	
	uint32_t inode_table_block = BGD[group].inode_table;
	index -= group * ext2_ramdisk_inodes_per_group;	// adjust index within group
	uint32_t block_offset = ((index - 1) * SB->inode_size) / BLOCKSIZE;
	uint32_t offset_in_block = (index - 1) - block_offset * (BLOCKSIZE / SB->inode_size);

	ext2_inodetable_t* inodet = kmalloc(BLOCKSIZE);

	ext2_ramdisk_read_block(inode_table_block + block_offset, (uint8_t*) inodet);
	memcpy(&inodet[offset_in_block], inode, sizeof(ext2_inodetable_t));
	ext2_ramdisk_write_block(inode_table_block + block_offset, (uint8_t*) inodet);

	kfree(inodet);
}

size_t write_ext2_ramdisk(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
	ext2_inodetable_t* inode = ext2_ramdisk_inode(node->inode);

	uint32_t end = offset + size;
	uint32_t start_block 	= offset / BLOCKSIZE;
	uint32_t end_block		= end / BLOCKSIZE;
	uint32_t end_size		= end - end_block * BLOCKSIZE;
	uint32_t size_to_write  = end - offset;

	kprintf("[kernel/ext2] Write at node 0x%x, offset %d, size %d, buffer=0x%x\n", node, offset, size, buffer);

	if (end_size == 0)
		end_block--;
	
	if (inode->size < end) {
		inode->size = end;
		ext2_ramdisk_write_inode(inode, node->inode);
	}
	
	if (start_block == end_block) {
		uint8_t* buf = kmalloc(BLOCKSIZE);
		ext2_ramdisk_inode_read_block(inode, node->inode, start_block, buf);
		memcpy((uint8_t*) ((uintptr_t) buf + (offset % BLOCKSIZE)), buffer, size_to_write);

		kprintf("[kernel/ext2] Single-block write.\n");
		ext2_ramdisk_inode_write_block(inode, node->inode, start_block, buf);

		kfree(buf);
		kfree(inode);
		return size_to_write;
	} else {
		uint32_t block_offset;
		uint32_t blocks_read = 0;

		for (block_offset = start_block; block_offset < end_block; block_offset++, blocks_read++) {
			if (block_offset == start_block) {
				void* buf = kmalloc(BLOCKSIZE);
				ext2_ramdisk_inode_read_block(inode, node->inode, block_offset, buf);
				memcpy((uint8_t*) ((uint32_t) buf + (offset % BLOCKSIZE)), buffer, BLOCKSIZE - (offset % BLOCKSIZE));

				kprintf("[kernel/ext2] Writing block [loop...]...\n");
				ext2_ramdisk_inode_write_block(inode, node->inode, start_block, buf);
				kfree(buf);
			} else {
				kprintf("[kernel/ext2] Writing block [buffer...?]...\n");
				ext2_ramdisk_inode_write_block(inode, node->inode, block_offset, 
						buffer + BLOCKSIZE * blocks_read - (block_offset % BLOCKSIZE));
			}
		}
		void* buf = kmalloc(BLOCKSIZE);
		ext2_ramdisk_inode_read_block(inode, node->inode, end_block, buf);

		memcpy(buf, buffer + BLOCKSIZE * blocks_read - (block_offset % BLOCKSIZE), end_size);
		kprintf("[kernel/ext2] Writing block [tail]...\n");
		ext2_ramdisk_inode_write_block(inode, node->inode, end_block, buf);
		kfree(buf);
	}

	kfree(inode);
	return size_to_write;
}

size_t read_ext2_ramdisk(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buffer) {
	ext2_inodetable_t* inode = ext2_ramdisk_inode(node->inode);

	uint32_t end;
	if (offset + size > inode->size)
		end = inode->size;
	else
		end = offset + size;

	uint32_t start_block  = offset / BLOCKSIZE;
	uint32_t end_block    = end / BLOCKSIZE;
	uint32_t end_size     = end - end_block * BLOCKSIZE;
	uint32_t size_to_read = end - offset;

	if (end_size == 0)
		end_block--;

	if (start_block == end_block) {
		uint8_t* buf = kcalloc(BLOCKSIZE, 1);
		ext2_ramdisk_inode_read_block(inode, node->inode, start_block, buf);
		memcpy(buffer, (uint8_t*) (((uint32_t) buf) + (offset % BLOCKSIZE)), size_to_read);

		kfree(buf);
		kfree(inode);
		return size_to_read;
	} else {
		uint32_t block_offset;
		uint32_t blocks_read = 0;

		for (block_offset = start_block; block_offset < end_block; block_offset++, blocks_read++) {
			if (block_offset == start_block) {
				uint8_t* buf = kmalloc(BLOCKSIZE);
				ext2_ramdisk_inode_read_block(inode, node->inode, block_offset, buf);
				memcpy(buffer, (uint8_t*) (((uint32_t) buf) + (offset % BLOCKSIZE)), BLOCKSIZE - (offset % BLOCKSIZE));
				kfree(buf);
			} else {
				uint8_t* buf = kmalloc(BLOCKSIZE);
				ext2_ramdisk_inode_read_block(inode, node->inode, block_offset, buf);
				memcpy(buffer + BLOCKSIZE * blocks_read - (offset % BLOCKSIZE), buf, BLOCKSIZE);
				kfree(buf);
			}
		}

	    uint8_t* buf = kmalloc(BLOCKSIZE);
		ext2_ramdisk_inode_read_block(inode, node->inode, end_block, buf);
		memcpy(buffer + BLOCKSIZE * blocks_read - (offset % BLOCKSIZE), buf, end_size);
	}

	kfree(inode);
	return size_to_read;
}

void open_ext2_ramdisk(fs_node_t* node, uint32_t flags) {
    (void) node;
    (void) flags;
}

void close_ext2_ramdisk(fs_node_t* node) {
    (void) node;
}

struct dirent* readdir_ext2_ramdisk(fs_node_t* node, uint32_t index) {
	ext2_inodetable_t* inode = ext2_ramdisk_inode(node->inode);
    if (!(inode->mode & EXT2_S_IFDIR)) {
        klog(LOG_ERR, "%s: inode passed is not a directory\n", __func__);
        return NULL;
    }

	ext2_dir_t* direntry = ext2_ramdisk_direntry(inode, node->inode, index);
	if (!direntry)
		return NULL;

	struct dirent* dirent = kmalloc(sizeof(struct dirent));
	memcpy(&dirent->name, &direntry->name, direntry->name_len);
	dirent->name[direntry->name_len] = '\0';
	dirent->inode = direntry->inode;

	kfree(direntry);
	kfree(inode);
	return dirent;
}

void insertdir_ext2_ramdisk(ext2_inodetable_t* p_node, uint32_t no, uint32_t inode, char* name, uint8_t type) {
	kprintf("[kernel/ext2] Request to insert new directory entry at 0x%x#%d->%d '%s' type %d\n", p_node, no, inode, name, type);

    if (!(p_node->mode & EXT2_S_IFDIR)) {
        klog(LOG_ERR, "%s: inode passed is not a directory\n", __func__);
        return;
    }

	void* block = kmalloc(BLOCKSIZE);
	uint32_t block_nr = 0;
	ext2_ramdisk_inode_read_block(p_node, no, block_nr, block);

	uint32_t dir_offset = 0;
	uint32_t total_offset = 0;

	while (total_offset < p_node->size) {
		ext2_dir_t* d_ent = (ext2_dir_t*) ((uintptr_t) block + dir_offset);

		if (d_ent->rec_len + total_offset == p_node->size) {
			d_ent->rec_len = d_ent->name_len + sizeof(ext2_dir_t);

			while (d_ent->rec_len % 4 > 0)
				d_ent->rec_len++;

			dir_offset   += d_ent->rec_len;
			total_offset += d_ent->rec_len;
			break;
		}

		dir_offset += d_ent->rec_len;
		total_offset += d_ent->rec_len;

		if (dir_offset >= BLOCKSIZE) {
			block_nr++;
			dir_offset -= BLOCKSIZE;
			ext2_ramdisk_inode_read_block(p_node, no, block_nr, block);
			kprintf("[kernel/ext2] Advancing to next block...\n");
		}
	}

	kprintf("[kernel/ext2] Total Offset = %d; block = %d; offset within block = %d\n", total_offset, block_nr, dir_offset);

	uint32_t size = p_node->size - total_offset;
	if (dir_offset + size > BLOCKSIZE)
		kprintf("[kernel/ext2] Just a warning: You probably just fucked everything\n");

	ext2_dir_t* new_entry = kmalloc(size);

	new_entry->inode = inode;
	new_entry->rec_len = size;
	new_entry->name_len = (uint8_t) strlen(name);
	new_entry->file_type = type;
	memcpy(&new_entry->name, name, strlen(name));

	memcpy(((uint8_t*) block) + dir_offset, new_entry, size);
	memset(((uint8_t*) block) + dir_offset + new_entry->rec_len, 0, 4);
	ext2_ramdisk_inode_write_block(p_node, no, block_nr, block);

	kfree(new_entry);

	ext2_ramdisk_write_inode(p_node, no);

	kfree(block);
}

fs_node_t* finddir_ext2_ramdisk(fs_node_t* node, char* name) {
	ext2_inodetable_t *inode = ext2_ramdisk_inode(node->inode);
    if (!(inode->mode & EXT2_S_IFDIR)) {
        klog(LOG_ERR, "%s: inode passed is not a directory\n", __func__);
        return NULL;
    }

	void* block = kmalloc(BLOCKSIZE);
	ext2_dir_t* direntry = NULL;
	uint8_t block_nr = 0;
	ext2_ramdisk_inode_read_block(inode, node->inode, block_nr, block);

	uint32_t dir_offset = 0;
	uint32_t total_offset = 0;

	while (total_offset < inode->size) {
		ext2_dir_t* d_ent = (ext2_dir_t*) ((uintptr_t) block + dir_offset);
		
		if (strlen(name) != d_ent->name_len) {
			dir_offset += d_ent->rec_len;
			total_offset += d_ent->rec_len;

			if (dir_offset >= BLOCKSIZE) {
				block_nr++;
				dir_offset -= BLOCKSIZE;
				ext2_ramdisk_inode_read_block(inode, node->inode, block_nr, block);
			}

			continue;
		}

		char* dname = kmalloc(sizeof(char) * (d_ent->name_len + 1));
		memcpy(dname, &(d_ent->name), d_ent->name_len);
		dname[d_ent->name_len] = '\0';

		if (!strcmp(dname, name)) {
			kfree(dname);
			direntry = malloc(d_ent->rec_len);
			memcpy(direntry, d_ent, d_ent->rec_len);
			break;
		}

		kfree(dname);

		dir_offset += d_ent->rec_len;
		total_offset += d_ent->rec_len;

		if (dir_offset >= BLOCKSIZE) {
			block_nr++;
			dir_offset -= BLOCKSIZE;
			ext2_ramdisk_inode_read_block(inode, node->inode, block_nr, block);
		}
	}

	kfree(inode);
	kfree(block);

	if (!direntry)
		return NULL;

	fs_node_t* outnode = kmalloc(sizeof(fs_node_t));
	inode = ext2_ramdisk_inode(direntry->inode);
	ext2_ramdisk_node_from_file(inode, direntry, outnode);

	kfree(direntry);
	kfree(inode);
	return outnode;
}

uint32_t ext2_ramdisk_node_from_file(ext2_inodetable_t* inode, ext2_dir_t* direntry, fs_node_t* fnode) {
	if (!fnode)
		return 0;

	fnode->inode = direntry->inode;
	strcpy(fnode->name, &direntry->name);

	fnode->length = inode->size;

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

    fnode->open    = &open_ext2_ramdisk;
    fnode->close   = &close_ext2_ramdisk;
	fnode->read    = &read_ext2_ramdisk;
	fnode->write   = &write_ext2_ramdisk;
	fnode->readdir = &readdir_ext2_ramdisk;
	fnode->finddir = &finddir_ext2_ramdisk;

	return 1;
}

uint32_t ext2_ramdisk_node_root(ext2_inodetable_t* inode, fs_node_t* fnode) {
	if (!fnode) 
		return 0;

	fnode->inode = 2;
    strcpy(fnode->name, "initrd");
	fnode->length = inode->size;

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

    fnode->open    = &open_ext2_ramdisk;
    fnode->close   = &close_ext2_ramdisk;
	fnode->read    = &read_ext2_ramdisk;
	fnode->write   = &write_ext2_ramdisk;
	fnode->readdir = &readdir_ext2_ramdisk;
	fnode->finddir = &finddir_ext2_ramdisk;

	return 1;
}

void ext2_ramdisk_sync(void) {
	for (uint32_t i = 0; i < CACHEENTRIES; ++i) {
		if (DC[i].dirty)
			ext2_flush_dirty(i);
	}
}

void ext2_ramdisk_mount(uint32_t offset) {
	_ramdisk_offset = offset;
	DC = kmalloc(sizeof(ext2_ramdisk_cache_entry_t) * CACHEENTRIES);
	SB = kmalloc(BLOCKSIZE);

	ext2_ramdisk_read_block(1, (uint8_t*) SB);
    if (SB->magic != EXT2_SUPER_MAGIC) {
        klog(LOG_ERR, "Valid EXT2 superblock not found\n");
        return;
    }

	if (SB->inode_size == 0)
		SB->inode_size = 128;

	BGDS = SB->blocks_count / SB->blocks_per_group;
	ext2_ramdisk_inodes_per_group = SB->inodes_count / BGDS;

	BGD = kmalloc(BLOCKSIZE);
	ext2_ramdisk_read_block(2, (uint8_t*) BGD);

	ext2_inodetable_t* root_inode = ext2_ramdisk_inode(2);

	RN = (fs_node_t*) kmalloc(sizeof(fs_node_t));
	if (!ext2_ramdisk_node_root(root_inode, RN)) {
        klog(LOG_ERR, "Could not create EXT2 root node\n");
        return;
    }

    fs_register(RN);
	klog(LOG_OK, "Mounted EXT2 ramdisk\n");
}

void ext2_ramdisk_forget_superblock(void) {
	kfree(SB);
}
