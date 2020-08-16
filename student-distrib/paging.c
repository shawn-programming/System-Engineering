#include "paging.h"

/*
init_paging

function:       initialize pages (enable video memory page and kernel page)
input:          nonee
output:         none
side effect:    change values in the page directory and the table
magic numbers:  none
*/
void paging_init(){
    int i;
    for(i=0; i<KB; i++){
        // intialize the page directory with all offsets set to 0 
        page_directory[i] = 0;
        page_table_video[i] = 0;
        page_vid_mem[i] = 0;
    }
    
    // initialize the video memory page
    page_directory[video_memory_PD] = (uint32_t)page_table_video | initial_flag_vid;

    page_table_video[video_memory_PT] = (uint32_t)video_mem | initial_flag_vid;

    // initialize terminal memory pages
    page_table_video[term_1_PT] = (uint32_t)term_1_mem | initial_flag_vid;
    page_table_video[term_2_PT] = (uint32_t)term_2_mem | initial_flag_vid;
    page_table_video[term_3_PT] = (uint32_t)term_3_mem | initial_flag_vid;
    // initialize the kernel page
    page_directory[kernel_PD] = kernel_address | initial_flag_ker;

    /*
    Enable Pages - the following code is retrieved from https://wiki.osdev.org/Paging

    function:       enable page directory
    input:          page directory's address
    output:         none
    side effect:    write values to cr0, cr3, cr4 registers
    magic numbers:  0x00000010 - enable PSE / 0x80000001 - set the paging (PG) and protection (PE) bits
    */
    asm volatile ("                 \n\
            movl %0, %%cr3          \n\
            movl %%cr4, %%ebx       \n\
            orl  $0x00000010, %%ebx \n\
            movl %%ebx, %%cr4       \n\
            movl %%cr0, %%ebx       \n\
            orl  $0x80000001, %%ebx \n\
            movl %%ebx, %%cr0       \n\
            "
            :
            : "r"(page_directory)
            : "ebx"
    );

}

/*
map_video_memory

function:       This creates a 4MB page table in memory and sets the first page to be the physical address of the video memory
input:          uint32_t virtual address -- the virtual address of the page table
                uint32_t physical address -- the physical adress of the video memory
output:         none
side effect:    
magic numbers:  none
*/
void map_video_memory(uint32_t virtual_addr, uint32_t phys_addr){
    page_directory[virtual_addr / MB4] = (uint32_t)page_table_video | vid_mem_flag;
    page_table_video[0] = phys_addr | vid_mem_flag;
    flush_tlb();
} 
