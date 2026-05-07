# mYOS

A simple custom x86 operating system written in C and Assembly.

---

## Features

- Custom kernel  
- VGA text-mode terminal  
- Keyboard input handling  
- Command shell  
- Command history  
- File system support  
- Persistent storage using ATA PIO disk driver  
- Directory support  
- Basic shell commands  
- Runs on QEMU  

---

##  Current Commands

### System Commands
- `help`
- `clear`
- `about`
- `color <0-15>`
- `reboot`
- `halt`

### File System Commands
- `ls`
- `touch <file>`
- `write <file> <text>`
- `cat <file>`
- `rm <file>`
- `mkdir <dir>`
- `cd <dir>`
- `pwd`

---

## Project Structure
```
mYOS/
│
├── src/
│ ├── boot.asm
│ ├── kernel.c
│ ├── keyboard.c
│ ├── idt.c
│ ├── fs.c
│ ├── disk.c
│ ├── ports.c
│ ├── linker.ld
│ ├── fs.h
│ ├── ports.h
│ └── idt.h
│
├── kernel.elf
├── mYOS.img
└── README.md
```

---

## Requirements

### Tools Required
- NASM  
- QEMU  
- x86_64-elf GCC Cross Compiler  

---

## Windows Setup

### NASM
Download: https://www.nasm.us/

### QEMU
Download: https://www.qemu.org/download/

### Cross Compiler
Use an `x86_64-elf` GCC cross compiler.

---

## Build Instructions

### Assemble Bootloader
```bash
nasm -f elf32 src\boot.asm -o boot.o
```
### Compile Kernel Sources
```bash
C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\kernel.c -o kernel.o
C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\keyboard.c -o keyboard.o
C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\idt.c -o idt.o
C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\fs.c -o fs.o
C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\disk.c -o disk.o
C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\ports.c -o ports.o
```
### Link Kernel
```bash
C:\cross\bin\x86_64-elf-ld.exe -m elf_i386 -T src\linker.ld -o kernel.elf boot.o kernel.o keyboard.o idt.o fs.o disk.o ports.o
```
### Creating Disk Image
```bash
fsutil file createnew mYOS.img 10485760
```
### Running mYOS
```bash
qemu-system-i386 -kernel kernel.elf -drive file=mYOS.img,format=raw
```

## Future Goals
•	Scrolling terminal
•	Better shell
•	FAT filesystem
•	Multitasking
•	Memory management
•	Paging
•	ELF executable loading
•	User mode
•	Networking
•	GUI

## FileSystem

mYOS contains a very small custom filesystem:

•  Fixed-size file table
•  Directory hierarchy
•  Persistent sector storage
•  ATA PIO disk access

## Author
Yashvi Soni 
