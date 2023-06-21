#include <vga.h>

static const size_t NUM_COLS = 80;
static const size_t NUM_ROWS = 25;

struct Char {
    uint8_t character;
    uint8_t color;
};

struct Char* screen_buffer = (struct Char*) 0xc03ff000;
size_t col = 0;
size_t row = 0;
uint8_t color = VGA_COLOR_WHITE | VGA_COLOR_BLACK << 4;

static inline void clear_row(size_t row) {
    struct Char empty = (struct Char) {
        character: ' ',
        color: color,
    };

    for (size_t col = 0; col < NUM_COLS; col++) {
        screen_buffer[col + NUM_COLS * row] = empty;
    }
}

void clear() {
    for (size_t i = 0; i < NUM_ROWS; i++) {
        clear_row(i);
    }
}

static inline void print_newline() { col = 0;

    if (row < NUM_ROWS - 1) {
        row++;
        return;
    }

    for (row = 1; row < NUM_ROWS; row++) {
        for (col = 0; col < NUM_COLS; col++) {
            struct Char character = screen_buffer[col + NUM_COLS * row];
            screen_buffer[col + NUM_COLS * (row - 1)] = character;
        }
    }

    clear_row(NUM_COLS - 1);
}

void print_char(char character) {
    if (character == '\n') {
        print_newline();
        return;
    }

    if (col > NUM_COLS) {
        print_newline();
    }

    screen_buffer[col + NUM_COLS * row] = (struct Char) {
        character: (uint8_t) character,
        color: color,
    };

    col++;
}

void print_string(char* string) {
    for (size_t i = 0; 1; i++) {
        char character = (uint8_t) string[i];

        if (character == '\0') {
            return;
        }

        print_char(character);
    }
}

void set_color(enum vga_color fg, enum vga_color bg) {
    color = fg | bg << 4;
}
