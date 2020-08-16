#ifndef _PCB_H
#define _PCB_H

#include "types.h"

//fops table
typedef struct fops {
    int32_t (*open_)(const uint8_t* filename);
    int32_t (*close_)(int32_t fd);
    int32_t (*read_)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write_)(int32_t fd, const void* buf, int32_t nbytes);
} __attribute__((packed)) fops_t;

// File_Array (16B);
typedef struct file_array {
    fops_t pointer_;
    uint32_t inode_;
    uint32_t file_position_;
    uint32_t flags_;
} __attribute__((packed)) file_array_t;

// process control block
typedef struct pcb {
    int32_t pid;            // unique id for each process
	int32_t parent_pid;     // pid of the parent pcb. -1 if this pcb has no parent
    int32_t halt_ebp;       // ebp value to be restored when halt is called
    int32_t halt_esp;       // esp value to be restored when halt is called
    int32_t ebp_val;        // ebp value to be restored when scheduler continues this process
    int32_t esp_val;        // esp value to be restored when scheduler continues this process
    int8_t* argument;       // parameter for the function
	file_array_t file_descriptor[8]; // Maximum # of functions can be open is 8
    struct __attribute__((packed)) pcb* parent_pcb; // pcb pointer to its parent
} __attribute__((packed)) pcb_t;

//terminal data structure holds a pcb, cursor positions
typedef struct terminal {
    pcb_t* pcb;
    uint32_t screen_x; //cursor x
    uint32_t screen_y; //cursor y
} __attribute__((packed)) terminal_t;

terminal_t terminal[3]; //there are three terminals
uint8_t current_terminal; //global variable for visible terminal
uint8_t curr_scheduler; //global variable for terminal that scheduling is processing
#endif
