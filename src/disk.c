
// #include <stdint.h>
// #include "ports.h"

// int disk_wait() {

//     int timeout = 100000;

//     while ((inb(0x1F7) & 0x80) && timeout--)
//         ;

//     timeout = 100000;

//     while (!(inb(0x1F7) & 0x08) && timeout--)
//         ;

//     if (timeout <= 0)
//         return 0;

//     return 1;
// }


// void disk_read_sector(uint32_t lba, uint8_t* buffer) {

//     disk_wait();

//     outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
//     outb(0x1F2, 1);
//     outb(0x1F3, (uint8_t)lba);
//     outb(0x1F4, (uint8_t)(lba >> 8));
//     outb(0x1F5, (uint8_t)(lba >> 16));
//     outb(0x1F7, 0x20);

//     disk_wait();

//     for (int i = 0; i < 256; i++) {
//         ((uint16_t*)buffer)[i] = inw(0x1F0);
//     }
// }

// void disk_write_sector(uint32_t lba, uint8_t* buffer) {

//     disk_wait();

//     outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
//     outb(0x1F2, 1);
//     outb(0x1F3, (uint8_t)lba);
//     outb(0x1F4, (uint8_t)(lba >> 8));
//     outb(0x1F5, (uint8_t)(lba >> 16));
//     outb(0x1F7, 0x30);

//     disk_wait();

//     for (int i = 0; i < 256; i++) {
//         outw(0x1F0, ((uint16_t*)buffer)[i]);
//     }
// }/


#include <stdint.h>
#include "ports.h"

#define ATA_DATA       0x1F0
#define ATA_SECCOUNT0  0x1F2
#define ATA_LBA0       0x1F3
#define ATA_LBA1       0x1F4
#define ATA_LBA2       0x1F5
#define ATA_HDDEVSEL   0x1F6
#define ATA_COMMAND    0x1F7
#define ATA_STATUS     0x1F7

#define ATA_CMD_READ   0x20
#define ATA_CMD_WRITE  0x30

#define ATA_SR_BSY     0x80
#define ATA_SR_DRQ     0x08

// void disk_wait_bsy() {
//     while (inb(ATA_STATUS) & ATA_SR_BSY);
// }

// void disk_wait_drq() {
//     while (!(inb(ATA_STATUS) & ATA_SR_DRQ));
// }


void disk_wait() {

    // 400ns delay
    for (int i = 0; i < 1000; i++) {
        inb(0x1F7);
    }

    // wait while busy
    while (inb(0x1F7) & 0x80);
}



void disk_read_sector(uint32_t lba, uint8_t* buffer) {

    disk_wait();

    outb(ATA_HDDEVSEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT0, 1);
    outb(ATA_LBA0, (uint8_t)(lba));
    outb(ATA_LBA1, (uint8_t)(lba >> 8));
    outb(ATA_LBA2, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND, ATA_CMD_READ);

    disk_wait();
    disk_wait();

    uint16_t* ptr = (uint16_t*)buffer;

    for (int i = 0; i < 256; i++) {
        ptr[i] = inw(ATA_DATA);
    }
}

void disk_write_sector(uint32_t lba, uint8_t* buffer) {

    disk_wait();

    outb(ATA_HDDEVSEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT0, 1);
    outb(ATA_LBA0, (uint8_t)(lba));
    outb(ATA_LBA1, (uint8_t)(lba >> 8));
    outb(ATA_LBA2, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND, ATA_CMD_WRITE);

    disk_wait();
    disk_wait();

    uint16_t* ptr = (uint16_t*)buffer;

    for (int i = 0; i < 256; i++) {
        outw(ATA_DATA, ptr[i]);
    }

    disk_wait();
}










