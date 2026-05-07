[bits 32]

global keyboard_isr
extern keyboard_handler

keyboard_isr:

    pusha

    call keyboard_handler

    mov al, 0x20
    out 0x20, al

    popa

    iretd


section .multiboot
align 4
    dd 0x1BADB002
    dd 0
    dd -(0x1BADB002)

section .text
global _start
extern kmain

_start:
    call kmain

hang:
    jmp hang