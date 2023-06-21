#include <drivers/vga.h>

static uint16_t* screen_buffer = (uint16_t*) 0xc00b8000;
static uint16_t color = (VGA_COLOR_BLACK << 4 | VGA_COLOR_WHITE) << 8;
static uint8_t cur_x = 0, cur_y = 0;

void vga_clear(void) {
	for (uint16_t i = 0; i < NUM_COLS * NUM_ROWS; i++) {
		*(screen_buffer + i) = (uint16_t) 3872; 
    }
}

static void scroll(void) {
    if(cur_y >= NUM_ROWS) {
        uint16_t temp = cur_y - NUM_ROWS + 1;
        memcpy(screen_buffer, screen_buffer + temp * NUM_COLS, (NUM_ROWS - temp) * NUM_COLS * 2);

        for(int8_t i = 0; i < NUM_COLS; ++i) {
            screen_buffer[NUM_COLS * (NUM_ROWS - 1) + i] = 3872; 
        }

        cur_y = NUM_ROWS - 1;
    }
}

static void move_cur(void) {
    uint16_t temp = cur_y * NUM_COLS + cur_x;

    outb(0x3D4, 14);
    outb(0x3D5, temp >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, temp);
}

void vga_putc(const char c) {
    uint16_t *where;

    if(c == 0x08) {
        if(cur_x != 0) {
            cur_x--;
            where = screen_buffer + (cur_y * NUM_COLS + cur_x);
            *where = 3872;
        } 
    } else if(c == 0x09) {
        cur_x = (cur_x + 8) & ~(8 - 1);
    } else if(c == '\r') {
        cur_x = 0;
    } else if(c == '\n') {
        cur_x = 0;
        cur_y++;
    } else if(c >= ' ') {
        where = screen_buffer + (cur_y * NUM_COLS + cur_x);
        *where = (uint16_t) c | color;	
        cur_x++;
    }

    if(cur_x >= NUM_COLS) {
        cur_x = 0;
        cur_y++;
    }

    scroll();
    move_cur();
}

void vga_puts(const char* string) {
    int32_t i = 0;

    while(string[i] != '\0') {
        vga_putc(string[i]);
        i++;
    }
}

void vga_set_color(enum vga_color fg, enum vga_color bg) {
    color = (bg << 4 | fg) << 8;
}
