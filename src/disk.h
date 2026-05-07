#pragma once
#include <stdint.h>

void disk_read_sector(uint32_t lba, uint8_t* buffer);
void disk_write_sector(uint32_t lba, uint8_t* buffer);