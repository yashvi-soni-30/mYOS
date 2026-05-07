#include <stdint.h>
#include "idt.h"
#include "ports.h"
#include "fs.h"

void print_prompt();

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

//volatile char* vga = (volatile char*)0xB8000;
volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

int cursor = 0;
uint8_t color = 0x07;

/* ================= CURSOR ================= */

void update_cursor() {
    uint16_t pos = cursor;

    outb(0x3D4, 0x0F);
    outb(0x3D5, pos & 0xFF);

    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8) & 0xFF);
}

/* ================= SCREEN ================= */

void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        // vga[i * 2] = vga_entry(' ', color);
        // vga[i * 2 + 1] = color;
        vga[i] = vga_entry(' ', color);
    }
    cursor = 0;
    update_cursor();
}

/* ================= PRINT ================= */

void putchar(char c) {

    if (c == '\n') {
        cursor += VGA_WIDTH - (cursor % VGA_WIDTH);
    } else if (c == '\r') {
        cursor -= (cursor % VGA_WIDTH);
    } else if (c == '\b') {
        if (cursor > 0) {
            cursor--;
            vga[cursor] = vga_entry(' ', color);
        }
    } else {
        vga[cursor] = vga_entry(c, color);
        cursor++;
    }

    if (cursor >= VGA_WIDTH * VGA_HEIGHT) {
        cursor = 0;  // we’ll upgrade this to scrolling later
    }

    update_cursor();
}

void print(const char* s) {
    for (int i = 0; s[i]; i++) putchar(s[i]);
}

int get_prompt_length() {
    int len = 0;

    len += 4; // "YOS:"
    
    // approximate fs_pwd length (safe fallback)
    len += 16; 
    
    len += 3; // " > "

    return len;
}

/* ================= STRING ================= */

int strcmp(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 1;
        i++;
    }
    return a[i] != b[i];
}

int startswith(const char* s, const char* p) {
    int i = 0;
    while (p[i]) {
        if (s[i] != p[i]) return 0;
        i++;
    }
    return 1;
}

/* ================= COMMANDS ================= */

void cmd_help() {
    print("\nAvailable commands:\n");

    /* basic */
    print("help   - show commands\n");
    print("clear  - clear screen\n");
    print("echo   - print text\n");
    print("about  - OS info\n");
    print("color  - change color (0-15)\n");

    /* system */
    print("reboot - restart system\n");
    print("halt   - stop CPU\n");

    /* filesystem */
    print("ls     - list files\n");
    print("touch  - create file (touch name)\n");
    print("write  - write to file (write name text)\n");
    print("cat    - show file contents\n");
    print("rm     - delete file\n");

    print("mkdir  - create directory\n");
    print("cd     - change directory\n");
    print("pwd    - show path\n");
}

void cmd_about() {
    print("\nYOS v1.0\n");
}

void cmd_echo(const char* input) {
    print("\n");
    for (int i = 5; input[i]; i++) putchar(input[i]);
}

void cmd_color(const char* input) {
    int val = input[6] - '0';
    if (val >= 0 && val <= 15) {
        color = val;
        print("\nColor changed\n");
    } else {
        print("\nInvalid color\n");
    }
}

void cmd_reboot() {
    outb(0x64, 0xFE);
}

void cmd_halt() {
    while (1) __asm__ volatile ("hlt");
}

/* ================= SHELL ================= */

#define MAX_INPUT 128
#define HISTORY 10

char input[MAX_INPUT];
int len = 0;
int pos = 0;

char history[HISTORY][MAX_INPUT];
int history_count = 0;
int history_index = 0;

/* redraw full line */
void redraw_line() {

    // move to beginning of current line
    // int row = cursor / VGA_WIDTH;
    // int line_start = row * VGA_WIDTH;
    int current_row = cursor / VGA_WIDTH;
    int line_start = current_row * VGA_WIDTH;

    // clear line safely
    for (int i = 0; i < VGA_WIDTH; i++) {
        vga[line_start + i] = vga_entry(' ', color);
    }

    // reset cursor to line start
    cursor = line_start;

    // print prompt
    print("YOS:/ > ");

    // print current input
    for (int i = 0; i < len; i++) {
        putchar(input[i]);
    }

    // restore cursor
    cursor = line_start + 8 + pos;

    update_cursor();
}
/* load history */
void load_history(int idx) {
    len = 0;
    pos = 0;

    for (int i = 0; history[idx][i]; i++) {
        input[i] = history[idx][i];
        len++;
    }
    input[len] = 0;
    pos = len;

    redraw_line();
}

/* execute command */
void execute_command() {

    if (strcmp(input, "help") == 0) cmd_help();
    //else if (strcmp(input, "clear") == 0) clear_screen();
    else if (strcmp(input, "clear") == 0) {
    clear_screen();
    print_prompt();
    return;
}
    else if (strcmp(input, "about") == 0) cmd_about();
    else if (startswith(input, "echo ")) cmd_echo(input);
    else if (startswith(input, "color ")) cmd_color(input);
    else if (strcmp(input, "reboot") == 0) cmd_reboot();
    else if (strcmp(input, "halt") == 0) cmd_halt();
    else if (startswith(input, "mkdir ")) fs_mkdir(input + 6);
    else if (startswith(input, "cd ")) fs_cd(input + 3);
    else if (strcmp(input, "pwd") == 0) {
    print("\n");
    fs_pwd();
}

    /* ---------- FILESYSTEM ---------- */

    else if (strcmp(input, "ls") == 0) fs_ls();

    else if (startswith(input, "touch ")) {
        fs_create(input + 6);
    }

    else if (startswith(input, "cat ")) {
        fs_cat(input + 4);
    }

    else if (startswith(input, "rm ")) {
        fs_rm(input + 3);
    }

    else if (startswith(input, "write ")) {

        int i = 6;
        char name[32];
        int j = 0;

        while (input[i] && input[i] != ' ') {
            name[j++] = input[i++];
        }
        name[j] = 0;

        if (input[i] == ' ') i++;

        fs_write(name, input + i);
    }

    /* ---------- UNKNOWN ---------- */

    else if (len > 0) {
        print("\nUnknown command");
    }

    len = 0;
    pos = 0;
    input[0] = 0;
}
/* ================= INPUT ================= */

void shell_input(char c) {

    if (len >= MAX_INPUT - 1)
        return;

    input[len] = c;
    len++;
    pos = len;

    input[len] = 0;

    redraw_line();
}


void shell_backspace() {

    if (len <= 0)
        return;

    len--;
    pos = len;

    input[len] = 0;

    redraw_line();
}

void shell_enter() {

    input[len] = 0;

    if (len > 0) {
        for (int i = 0; i <= len; i++)
            history[history_count % HISTORY][i] = input[i];

        history_count++;
        history_index = history_count;
    }

    execute_command();
    putchar('\n'); 
    print_prompt();   // <-- important

    len = 0;
    pos = 0;
    input[0] = 0;
}

void shell_move_left() {
    if (pos > 0) {
        pos--;
        cursor--;
        update_cursor();
    }
}

void shell_move_right() {
    if (pos < len) {
        pos++;
        cursor++;
        update_cursor();
    }
}

void shell_history_up() {
    if (history_count == 0) return;
    if (history_index > 0) history_index--;
    load_history(history_index % HISTORY);
}

void shell_history_down() {
    if (history_count == 0) return;
    if (history_index < history_count - 1) history_index++;
    load_history(history_index % HISTORY);
}

/* ================= INIT ================= */

void clear_keyboard_buffer() {
    while (inb(0x64) & 1) inb(0x60);
}

void print_prompt() {
    print("\nYOS:");

    fs_pwd();   // print current path

    print(" > ");
}

/* ================= MAIN ================= */

void kmain() {
    
    clear_screen();
    fs_init();
    print("Yashvi OS Started");
    clear_keyboard_buffer();
    idt_init();

    __asm__ volatile ("sti");
    print_prompt();
   
    while (1) {
        __asm__ volatile ("hlt");
    }
}