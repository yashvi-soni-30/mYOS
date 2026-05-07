mYOS
A simple custom x86 operating system written in C and Assembly.
Features
•	Custom kernel
•	VGA text-mode terminal
•	Keyboard input handling
•	Command shell
•	Command history
•	File system support
•	Persistent storage using ATA PIO disk driver
•	Directory support
•	Basic shell commands
•	Runs on QEMU
________________________________________
Current Commands
System
help
clear
about
color <0-15>
reboot
halt
File System
ls
touch <file>
write <file> <text>
cat <file>
rm <file>
mkdir <dir>
cd <dir>
pwd
________________________________________
Project Structure
mYOS/
│
├── src/
│   ├── boot.asm
│   ├── kernel.c
│   ├── keyboard.c
│   ├── idt.c
│   ├── fs.c
│   ├── disk.c
│   ├── ports.c
│   ├── linker.ld
│   ├── fs.h
│   ├── ports.h
│   └── idt.h
│
├── kernel.elf
├── myos.img
└── README.md
________________________________________
Requirements
Tools Required
•	NASM
•	QEMU
•	x86_64-elf GCC Cross Compiler
Windows Setup
NASM
Install from:
https://www.nasm.us/
QEMU
Install from:
https://www.qemu.org/download/
Cross Compiler
Use an x86_64-elf GCC cross compiler.
________________________________________
Build Instructions
Assemble Bootloader
nasm -f elf32 src\boot.asm -o boot.o
Compile Kernel Sources
C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\kernel.c -o kernel.o

C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\keyboard.c -o keyboard.o

C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\idt.c -o idt.o

C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\fs.c -o fs.o

C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\disk.c -o disk.o

C:\cross\bin\x86_64-elf-gcc.exe -m32 -ffreestanding -c src\ports.c -o ports.o
Link Kernel
C:\cross\bin\x86_64-elf-ld.exe -m elf_i386 -T src\linker.ld -o kernel.elf boot.o kernel.o keyboard.o idt.o fs.o disk.o ports.o
________________________________________
Creating the Disk Image
Create 10 MB Raw Disk Image
fsutil file createnew mYOS.img 10485760
________________________________________
Running mYOS
qemu-system-i386 -kernel kernel.elf -drive file=mYOS.img,format=raw
________________________________________

Future Goals
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
________________________________________
Author
Developed by Yashvi Soni Computer Science and Engineering

________________________________________
License

This project is for educational and learning purposes.