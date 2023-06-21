#include <drivers/vga.h>

static const uint8_t NUM_COLS = 80;
static const uint8_t NUM_ROWS = 25;

static uint16_t* screen_buffer = (uint16_t*) 0xc03ff000;
static uint16_t cursor = 0;
static uint16_t color = (VGA_COLOR_BLACK << 4 | VGA_COLOR_WHITE) << 8;

void vga_clear(void) {
	for (int i = 0; i < NUM_COLS * NUM_ROWS; i++) {
		*(screen_buffer + i) = (uint16_t) 3872; 
    }
}

static inline void vga_scroll(void) {
    int i = 0;

	for (i = 0; i < NUM_COLS * (NUM_ROWS - 1); ++i) {
		screen_buffer[i] = screen_buffer[i + NUM_COLS];
    }

	for(i = 0; i < NUM_COLS; ++i) {
		screen_buffer[NUM_COLS * (NUM_ROWS - 1) + i] = 3872; 
    }
}

void vga_puts(const char* string) {
	int i = 0;

	while (string[i] != '\0') {
		if (cursor == NUM_COLS * NUM_ROWS) {
			vga_scroll();
			cursor = NUM_COLS * (NUM_ROWS - 1);
		}

		switch (string[i]) {
            case '\n':
                cursor = cursor + NUM_COLS - cursor % NUM_COLS;
                break;
            case '\r':
                cursor = cursor - cursor % NUM_COLS;
                break;
            case '\t':
             	while ((cursor % NUM_COLS) % 8 != 0)
				    cursor++;
                break;
            default:
                screen_buffer[cursor] = (uint16_t) (color | string[i]);
                cursor++;
	    }
		i++;
	}
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    color = (bg << 4 | fg) << 8;
}
