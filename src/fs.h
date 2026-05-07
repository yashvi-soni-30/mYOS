// #pragma once

// #define MAX_FILES 32
// #define MAX_FILE_NAME 32
// #define MAX_FILE_SIZE 512

// #define FS_MAGIC 0x1234ABCD

// typedef struct {
//     uint32_t magic;
//     File files[MAX_FILES];
// } FileSystem;

// typedef struct {
//     char name[MAX_FILE_NAME];
//     char data[MAX_FILE_SIZE];
//     int size;
//     int used;
// } File;

// void fs_init();
// void fs_ls();
// void fs_create(const char* name);
// void fs_write(const char* name, const char* content);
// void fs_cat(const char* name);
// void fs_rm(const char* name);

#pragma once
#include <stdint.h>

/* config */
#define MAX_FILES 4
#define MAX_NAME  16
#define MAX_DATA  32

extern int current_dir;
/* file structure */
typedef struct {
    int used;
    int is_dir;        // 1 = directory, 0 = file
    int parent;        // index of parent directory
    char name[MAX_NAME];
    char data[MAX_DATA];
    int size;
} File;

/* filesystem structure */
#define FS_MAGIC 0x1234ABCD

typedef struct {
    uint32_t magic;
    File files[MAX_FILES];
} FileSystem;

/* functions */
void fs_init();
void fs_ls();
void fs_create(const char* name);
void fs_write(const char* name, const char* content);
void fs_cat(const char* name);
void fs_rm(const char* name);
void fs_mkdir(const char* name);
void fs_cd(const char* name);
void fs_pwd();