#include <drivers/keyboard.h>

static uint8_t shiftcode[256] = {
  [0x1D] = CTL,
  [0x2A] = SHIFT,
  [0x36] = SHIFT,
  [0x38] = ALT,
  [0x9D] = CTL,
  [0xB8] = ALT
};

static uint8_t togglecode[256] = {
  [0x3A] = CAPSLOCK,
  [0x45] = NUMLOCK,
  [0x46] = SCROLLLOCK
};

static uint8_t normalmap[256] = {
  0,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',
  '7',  '8', '9',  '0',  '-',  '=',  '\b', '\t',
  'q',  'w', 'e',  'r',  't',  'y',  'u',  'i',
  'o',  'p', '[',  ']',  '\n',  0,   'a',  's',
  'd',  'f', 'g',  'h',  'j',  'k',  'l',  ';',
  '\'', '`',  0,   '\\', 'z',  'x',  'c',  'v',
  'b',  'n', 'm',  ',',  '.',  '/',   0,   '*',
   0,   ' ',  0,    0,    0,    0,    0,    0,
   0,    0,   0,    0,    0,    0,    0,   '7',
  '8',  '9', '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3', '0',  '.',   0,    0,    0,    0,
  [0x9C] = '\n',
  [0xB5] = '/',
};

static uint8_t shiftmap[256] = {
  0,   033,  '!',  '@',  '#',  '$',  '%',  '^',
  '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
  'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
  'O',  'P',  '{',  '}',  '\n', 0,   'A',  'S',
  'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
  '"',  '~',   0,   '|',  'Z',  'X',  'C',  'V',
  'B',  'N',  'M',  '<',  '>',  '?',  0,   '*',
   0,   ' ',   0,   0,   0,   0,   0,   0,
   0,    0,    0,   0,   0,   0,   0,   '7',
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  0,   0,   0,   0,
  [0x9C] = '\n',
  [0xB5] = '/',
};

static uint8_t ctlmap[256] = {
  0,       0,       0,       0,       0,       0,       0,       0,
  0,       0,       0,       0,       0,       0,       0,       0,
  C('Q'),  C('W'),  C('E'),  C('R'),  C('T'),  C('Y'),  C('U'),  C('I'),
  C('O'),  C('P'),  0,       0,      '\r',     0,       C('A'),  C('S'),
  C('D'),  C('F'),  C('G'),  C('H'),  C('J'),  C('K'),  C('L'),  0,
  0,       0,       0,       C('\\'), C('Z'),  C('X'),  C('C'),  C('V'),
  C('B'),  C('N'),  C('M'),  0,       0,       C('/'),  0,       0,
  [0x9C] = '\r',
  [0xB5] = C('/'),
};

static uint8_t shift;
static uint8_t* charcode[4] = { normalmap, shiftmap, ctlmap, ctlmap };

static void keyboard_irq_handler(regs_t* r) {
    uint8_t data, c;

    for (uint16_t i = 0; i < 1000; i++)
        if((inb(PS2_CMD) & 1) == 0) continue;

    data = inb(PS2_DATA);

    if(data == 0xE0){
        shift |= E0ESC;
        return;
    } else if(data & 0x80){
        data = (shift & E0ESC ? data : data & 0x7F);
        shift &= ~(shiftcode[data] | E0ESC);
        return;
    } else if(shift & E0ESC){
        data |= 0x80;
        shift &= ~E0ESC;
    }

    shift |= shiftcode[data];
    shift ^= togglecode[data];
    c = charcode[shift & (CTL | SHIFT)][data];

    if(shift & CAPSLOCK) {
        if('a' <= c && c <= 'z')
            c += 'A' - 'a';
        else if('A' <= c && c <= 'Z')
            c += 'a' - 'A';
    }

    kprintf("%c", c);
    (void) r;
}

static void keyboard_reset(void) {
    uint8_t tmp = inb(0x61);
    outb(0x61, tmp | 0x80);
    outb(0x61, tmp & 0x7F);
    inb(PS2_DATA);
}

void keyboard_init(void) {
    /* installs keyboard_handler to IRQ1 */ 
    int_install_handler(KEYBOARD_IRQ, keyboard_irq_handler);
    keyboard_reset();
    klog(LOG_OK, "PS/2 Keyboard device initialized\n");
}

