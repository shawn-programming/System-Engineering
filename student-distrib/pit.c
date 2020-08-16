#include "pit.h"

/* 
 * pit_init
 *   DESCRIPTION: Initialize the PIT
 *   to the ports.
 *   INPUTS: none
 *   OUTPUTS: none
 */

void pit_init(){
    outb(PIT_MODE, PIT_CMD); //set the command byte
    outb((int8_t)PIT_FREQ, PIT_DATA); //set low byte
    outb((int8_t)PIT_FREQ >> SHIFT_8, PIT_DATA); //set high byte
    enable_irq(PIT_IRQ); //allows the program to sleep
}

/*
    pit_handler
    function:       this function calls scheduler
    input:          none
    output:         none
    side effect:    none
    magic numbers:  none
*/
void pit_handler(){
    scheduler();
}

// global var for current scheduler
uint8_t curr_scheduler = 0;

/*
    scheduler
    function:       launch a shell if there is no program for the corresponding terminal,
                    else, allow the multi-terminal function by scheduling
    input:          none
    output:         none
    side effect:    changes in the pagings of video meory and user space
    magic numbers:  none
*/
void scheduler(){
    // send the eoi signal so that other interrupts can occur
    send_eoi(PIT_IRQ);    
    cli();

    // if there no program running, run the very first shell
    if(curr_scheduler == 0 && (terminal[curr_scheduler].pcb == NULL)){
        sti();
        clear();
        clear_set();
        execute((uint8_t*)"shell");
    }

    // save the current scheduler's esp and ebp value
    asm volatile("                     \n\
    	          movl %%ebp, %0       \n\
                  movl %%esp, %1       \n\
                 "
                 : "=r"(terminal[curr_scheduler].pcb->ebp_val), "=r"(terminal[curr_scheduler].pcb->esp_val)
                 :
                 : "memory"
    );
    

    // update current scheduler index
    curr_scheduler = ((curr_scheduler + 1) % MAX_TERMINAL);

    // only allows the keyboard to write on the current terminal
    if(curr_scheduler == current_terminal){
        enable_irq(KEYBOARD_CONTROLLER_IRQ);
    }
    else{
        disable_irq(KEYBOARD_CONTROLLER_IRQ);
    }
    
    // if there is no running shell for this terminal, start one
    if(terminal[curr_scheduler].pcb == NULL){
        // terminal_switch(curr_scheduler);
        sti();
        execute((uint8_t*)"shell");
    }
         
    // adjust vid_map for the terminal
    if(curr_scheduler == current_terminal){
        page_table_video[0] = (uint32_t)video_mem | vid_mem_flag;
    }
    else{
        page_table_video[0] = (uint32_t)vid_page[curr_scheduler] | vid_mem_flag;
    }

    // adjust the paging of the user program
    page_directory[VIRTUAL_MEMORY_START] = (EIGHT_MB + (terminal[curr_scheduler].pcb->pid * FOUR_MB)) | PAGING_USER_ON; 
    flush_tlb();


    // adjust kernel stack
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - terminal[curr_scheduler].pcb->pid * EIGHT_KB) - FOUR_BYTE;

    // restore the current terminal's previous esp and ebp value
    asm volatile ("                 \n\
            movl %0, %%ebp          \n\
            movl %1, %%esp          \n\
            "
            :
            :"r"(terminal[curr_scheduler].pcb->ebp_val), "r"(terminal[curr_scheduler].pcb->esp_val)
            :"memory"
    );
    sti();
}
