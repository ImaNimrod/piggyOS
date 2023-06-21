#include <drivers/io_port.h>
#include <fs/devfs.h>

static size_t read_port(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf) {
	switch (size) {
		case 1:
			buf[0] = inb(offset);
			break;
		case 2:
			((uint16_t*) buf)[0] = inw(offset);
			break;
		case 4:
			((uint32_t*) buf)[0] = inl(offset);
			break;
		default:
			for (size_t i = 0; i < size; i++) 
				buf[i] = inb(offset + i);
			break;
	}

	return size;
}

static size_t write_port(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf) {
	switch (size) {
		case 1:
			outb(offset, buf[0]);
			break;
		case 2:
			outw(offset, ((uint16_t*) buf)[0]);
			break;
		case 4:
			outl(offset, ((uint32_t*) buf)[0]);
			break;
		default:
			for (size_t i = 0; i < size; ++i)
				outb(offset +i, buf[i]);
			break;
	}

	return size;
}

void port_device_create(void) {
    fs_node_t* port = (fs_node_t*) kcalloc(sizeof(fs_node_t), 1);

	strcpy(port->name, "port");
	port->flags = FS_CHARDEVICE;

    port->open = NULL;
    port->close = NULL;
    port->read = &read_port;
    port->write = &write_port;
    port->ioctl = NULL;

    devfs_register(port);
}
