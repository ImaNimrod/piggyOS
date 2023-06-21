#include <fs/devfs.h>

#define PHI 0x9e3779b9

static uint32_t Q[4096], c = 362436;

static void init_rand(uint32_t x) { /* initialize random number generator */
    Q[0] = x;
    Q[1] = x + PHI;
    Q[2] = x + PHI + PHI;

    for (int i = 3; i < 4096; i++)
        Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
}

static uint32_t __attribute__((pure)) rand(void) {
    uint64_t t, a = 18782LL;
    static uint32_t i = 4095;
    uint32_t x, r = 0xfffffffe;

    i = (i + 1) & 4095;
    t = a * Q[i] + c;
    c = (t >> 32);
    x = t + c;

    if (x < c) {
        x++;
        c++;
    }

    return (Q[i] = r - x);
}

static ssize_t write_random(fs_node_t* node, off_t offset, size_t size, uint8_t* buffer) {
    (void) node;
    (void) offset;
    (void) buffer;
	return size;
}

static ssize_t read_random(fs_node_t* node, off_t offset, size_t size, uint8_t* buffer) {
    (void) node;

    uint32_t s = 0;

	while (s < size) {
		buffer[s] = rand();
		offset++;
		s++;
	}

	return size;
}

void random_device_create(void) {
    fs_node_t* rand = (fs_node_t*) kmalloc(sizeof(fs_node_t));

    strcpy(rand->name, "rand");
    rand->flags = FS_CHARDEVICE;
    rand->length = 1024;
    rand->inode = 0;

    rand->atime = get_seconds();
    rand->ctime = rand->atime;

    rand->open = NULL;
    rand->close = NULL;
    rand->read = &read_random;
    rand->write = &write_random;
    rand->ioctl = NULL;

    init_rand(get_seconds());

    devfs_register(rand);
}
