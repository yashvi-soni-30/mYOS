#include "fs.h"
#include "disk.h"

/* external print */
// extern void print(const char* s);
// extern void putchar(char c);
// extern int strcmp(const char* a, const char* b);
// extern void strcpy(char*, const char*);
extern void print(const char*);
extern int strcmp(const char*, const char*);
extern void strcpy(char*, const char*);

FileSystem fs;
int current_dir = 0; // root


#define FS_START_LBA 1

void fs_save() {

    uint8_t sector[512];

    uint8_t* src = (uint8_t*)&fs;

    int total = sizeof(fs);
    int sectors = (total + 511) / 512;

    for (int s = 0; s < sectors; s++) {

        // clear temp buffer
        for (int i = 0; i < 512; i++)
            sector[i] = 0;

        // copy safely
        for (int i = 0; i < 512; i++) {

            int idx = s * 512 + i;

            if (idx < total)
                sector[i] = src[idx];
        }

        disk_write_sector(FS_START_LBA + s, sector);
    }
}




void fs_load() {

    uint8_t sector[512];

    uint8_t* dst = (uint8_t*)&fs;

    int total = sizeof(fs);
    int sectors = (total + 511) / 512;

    for (int s = 0; s < sectors; s++) {

        disk_read_sector(FS_START_LBA + s, sector);

        for (int i = 0; i < 512; i++) {

            int idx = s * 512 + i;

            if (idx < total)
                dst[idx] = sector[i];
        }
    }
}




void strcpy(char* dst, const char* src) {
    int i = 0;
    while (src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
}

/* ---------- core ---------- */
void fs_init() {
    fs_load();

 
    if (fs.magic != FS_MAGIC) {

        for (int i = 0; i < MAX_FILES; i++)
            fs.files[i].used = 0;

        // create root directory
        fs.files[0].used = 1;
        fs.files[0].is_dir = 1;
        fs.files[0].parent = -1;
        strcpy(fs.files[0].name, "/");

        fs.magic = FS_MAGIC;
        fs_save();
    }
    current_dir = 0;
}

void fs_ls() {
    print("\n");

    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && fs.files[i].parent == current_dir) {

            if (fs.files[i].is_dir)
                print("[DIR] ");
            else
                print("[FILE] ");

            print(fs.files[i].name);
            print("\n");
        }
    }
}

void fs_mkdir(const char* name) {

    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs.files[i].used) {

            fs.files[i].used = 1;
            fs.files[i].is_dir = 1;
            fs.files[i].parent = current_dir;
            strcpy(fs.files[i].name, name);

            print("\nDirectory created\n");
            fs_save();
            return;
        }
    }

    print("\nNo space\n");
}

void fs_cd(const char* name) {

    if (strcmp(name, "/") == 0) {
        current_dir = 0;
        return;
    }

    if (strcmp(name, "..") == 0) {
        if (current_dir != 0)
            current_dir = fs.files[current_dir].parent;
        return;
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used &&
            fs.files[i].is_dir &&
            fs.files[i].parent == current_dir &&
            strcmp(fs.files[i].name, name) == 0) {

            current_dir = i;
            return;
        }
    }

    print("\nDirectory not found\n");
}
void fs_pwd() {
    int stack[16];
    int top = 0;

    int cur = current_dir;

    while (cur != -1) {
        stack[top++] = cur;
        cur = fs.files[cur].parent;
    }

    // root case
    if (top == 1) {
        print("/");
        return;
    }

    for (int i = top - 2; i >= 0; i--) {
        print("/");
        print(fs.files[stack[i]].name);
    }
}

void fs_create(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs.files[i].used) {
            fs.files[i].used = 1;
            fs.files[i].size = 0;
            fs.files[i].is_dir = 0;
            fs.files[i].parent = current_dir;
            strcpy(fs.files[i].name, name);
            print("\nCreated file\n");
            fs_save();
            return;
        }
    }
    print("\nNo space\n");
    
}

void fs_write(const char* name, const char* content) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && strcmp(fs.files[i].name, name) == 0) {

            int j = 0;
            while (content[j] && j < MAX_DATA - 1) {
                fs.files[i].data[j] = content[j];
                j++;
            }

            fs.files[i].data[j] = 0;
            fs.files[i].size = j;

            print("\nWritten\n");
            fs_save();
            return;
        }
    }
    print("\nFile not found\n");
}

void fs_cat(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && strcmp(fs.files[i].name, name) == 0) {
            print("\n");
            print(fs.files[i].data);
            return;
        }
    }
    print("\nFile not found\n");
}

void fs_rm(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && strcmp(fs.files[i].name, name) == 0) {
            fs.files[i].used = 0;
            print("\nDeleted\n");
            fs_save();
            return;
        }
    }
    print("\nFile not found\n");
}