#ifndef _PAGING_H
#define _PAGING_H

#include "x86_desc.h"
#include "lib.h"
#include "system_call.h"
#include "system_call_asm.h"

#define video_mem 0xB8000 //the start of video memory
#define term_1_mem 0xB9000 //the start of terminal 1 memory
#define term_2_mem 0xBA000 //the start of terminal 2 memory
#define term_3_mem 0xBB000 //the start of terminal 3 memory


#define video_memory_PD 0 // video memory is located at the 0th index in the page directory
#define kernel_PD 1 // kernel is located at the 1st index in the page directory
#define video_memory_PT 0xB8 // video memory is located at the 0xB8th index in the page table
#define term_1_PT 0xB9 // terminal 1 memory is located at the 0xBCth index in the page table
#define term_2_PT 0xBA // terminal 2 memory is located at the 0xC0th index in the page table
#define term_3_PT 0xBB // terminal 3 memory is located at the 0xC3th index in the page table
#define initial_flag_vid 0x03 // Read/Write set to high, Present set to high, everything else set to low
#define initial_flag_ker 0x83 // Read/Write set to high, Present set to high, Page Size high, everything else set to low
#define kernel_address 0x00400000 // the starting address of the kernel
#define MB4 0x400000    // four mega bytes
#define vid_mem_flag 0x07 // Read/Write set to high, Present set to high, user to high, everything else set to low

uint32_t page_directory[KB] __attribute__((aligned(KB4)));
uint32_t page_table_video[KB] __attribute__((aligned(KB4)));
uint32_t page_vid_mem[KB] __attribute__((aligned(KB4)));

/* Initialize paging */
void paging_init();
void map_video_memory(uint32_t virtual_addr, uint32_t phys_addr);

#endif
