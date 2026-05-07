
#include <stdint.h>
#include "ports.h"

/* --- simple US set 1 scancode map (only what we need) --- */

/* ================= KEYBOARD MAP ================= */

char keyboard_map[128] = {

    0,
    27, // ESC
    '1','2','3','4','5','6','7','8','9','0',
    '-','=','\b',
    '\t',

    'q','w','e','r','t','y','u','i','o','p',
    '[',']',
    '\n',

    0, // CTRL

    'a','s','d','f','g','h','j','k','l',
    ';','\'','`',

    0, // SHIFT

    '\\',

    'z','x','c','v','b','n','m',
    ',', '.', '/',

    0, // SHIFT

    '*',

    0, // ALT
    ' ',

    0, // CAPS

    0,0,0,0,0,0,0,0,0,0,

    0, // NUMLOCK
    0, // SCROLLLOCK

    0,0,0,0,0,0,0,0,0,0,

    '-',0,0,0,'+',

    0,0,0,0,0,0,0,

    0,0,0
};

/* extended scancode prefix */
static int extended = 0;

/* provided by kernel.c */
void shell_input(char c);
void shell_backspace(void);
void shell_enter(void);
void shell_move_left(void);
void shell_move_right(void);
void shell_history_up(void);
void shell_history_down(void);

void keyboard_handler() {

    if (!(inb(0x64) & 1))
        return;

    uint8_t scancode = inb(0x60);

    // ignore key releases
    if (scancode & 0x80)
        return;

    switch (scancode) {

        case 0x1C:
            shell_enter();
            return;

        case 0x0E:
            shell_backspace();
            return;

        case 0x4B:
            shell_move_left();
            return;

        case 0x4D:
            shell_move_right();
            return;

        case 0x48:
            shell_history_up();
            return;

        case 0x50:
            shell_history_down();
            return;
    }
    if (scancode > 127) {
    outb(0x20, 0x20);
    return;
}

char c = keyboard_map[scancode];

    //char c = keyboard_map[scancode];

    if (c)
        shell_input(c);
}

