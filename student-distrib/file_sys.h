#ifndef _FILE_SYS_H
#define _FILE_SYS_H

#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "pcb.h"

#define MAX_LEN 32
#define FILE_NUM 62
#define BYTE 8
#define INDEX_2 2
#define INDEX_32 32
#define STRNCMP_32 32
#define SUCCESS 0
#define FAILURE -1
#define FILE_DESCRIPTOR_SIZE 8
#define PERIOD "."
#define NULL_CHAR "\0"
#define NULL_TERM '\0'

// first 64 bytes of the boot block
typedef struct file_stat {
    uint32_t dir_entry_num_;
    uint32_t inode_num_;
    uint32_t data_block_num_;
    uint32_t reserved_[13]; // 13 4byte chuncks are reserved
} __attribute__((packed)) file_stat_t;

// remaining 64 byte chuncks for the rest
typedef struct dentry {
    uint8_t file_name_[MAX_LEN];
    uint32_t file_type_;
    uint32_t inode_num_;
    uint32_t reserved_[6];  // 6 4byte chunks are reserved
} __attribute__((packed)) dentry_t;

// first block of the file system (4kb)
typedef struct boot_block {
    file_stat_t file_stat_;
    dentry_t dir_dentry_;
    dentry_t dentry_[FILE_NUM]; // # of dentry incluces "." (current directory)
} __attribute__((packed)) boot_block_t;

// inode block (4kb);
typedef struct inode {
    uint32_t length_;
    uint32_t data_block_[KB-1]; // excluding length (number of data blocks)
} __attribute__((packed)) inode_t;

// data block (4kb);
typedef struct data_block {
    uint8_t data[KB4];
} __attribute__((packed)) data_block_t;

boot_block_t* boot_block_addr;
inode_t* inode_addr;
data_block_t* data_block_addr;

void file_sys_init(boot_block_t* start_addr);
extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

extern int32_t file_open(const uint8_t* fname);
extern int32_t file_close(int32_t fd);
extern int32_t file_write(int32_t fd, void const* buf, int32_t nbytes);
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

extern int32_t dir_open(const uint8_t* fname);
extern int32_t dir_close(int32_t fd);
extern int32_t dir_write(int32_t fd, void const* buf, int32_t nbytes);
extern int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);

extern int32_t invalid_write_op(int32_t fd, void const* buf, int32_t nbytes);
extern int32_t invalid_read_op(int32_t fd, void* buf, int32_t nbytes);

#endif
