#include "system_call.h"

int32_t pid[SIX_BYTE] = {VACANT, VACANT, VACANT, VACANT, VACANT, VACANT};
uint32_t global_halt = 0;
uint8_t global_status = 0;
int8_t magic_identifiers[FOUR_BYTE] = {MAGIC_1, MAGIC_2, MAGIC_3, MAGIC_4};
static int process_counter = NEG_ONE;
static fops_t stdin_fops, stdout_fops, rtc_fops, dir_fops, file_fops;

/*
    halt(uint8_t status)

    function:       terminates a process && restore the parent page
    input:          status -- the status of the current process
    output:         -1 on failure, 0 on success
    */
int32_t halt(uint8_t status){

    // if we are exiting the initial shell, exit then restart the shell
    pid[terminal[curr_scheduler].pcb->pid] = VACANT;

    if (terminal[curr_scheduler].pcb->parent_pid == -1){
        terminal[curr_scheduler].pcb = NULL;
    	process_counter--;
    	execute((uint8_t*)"shell");
    }
    
    /* restore paging */
    page_directory[VIRTUAL_MEMORY_START] = (EIGHT_MB + (terminal[curr_scheduler].pcb->parent_pid * FOUR_MB)) | PAGING_USER_ON; 
    flush_tlb();
    tss.esp0 = (EIGHT_MB - terminal[curr_scheduler].pcb->parent_pid * EIGHT_KB) - FOUR_BYTE;

    asm volatile ("                 \n\
            movl %0, %%ebp          \n\
            movl %1, %%esp          \n\
            "
            :
            :"r"(terminal[curr_scheduler].pcb->parent_pcb->halt_ebp), "r"(terminal[curr_scheduler].pcb->parent_pcb->halt_esp)
    );
    int i;
    // close all of the files in the file descriptor
    for(i = 0; i <= END_OF_FDA; i++){
        if(terminal[curr_scheduler].pcb->file_descriptor[i].flags_ == 1){
            close(i);
        }
    }
    terminal[curr_scheduler].pcb = terminal[curr_scheduler].pcb->parent_pcb;
    process_counter--;
        /* return to execute */
    global_status = status;
    asm volatile("                     \n\
    	          movl $0, %%eax       \n\
                  jmp return           \n\
                 "
                 : /* no outputs */
                 : "r"(status));
    return -1; //never going to hit
}


/*
    execute(const uint8_t* command)

    function:       loads and executes a given program
    input:          command - the command string
    output:         -1 on failure, 0 on success
*/
int32_t execute(const uint8_t* command){
    cli();
    uint8_t filename[MAX_FILE_LEN];
    uint8_t arg[MAX_ARG_LEN]; 
    
    // checking for valid commands and parsing the arguments
    if(parse(command, filename, arg) == FAILURE) 
        return FAILURE;
    
    // if the number of processor is greater than 5, return failure
    if(terminal[curr_scheduler].pcb != NULL && process_counter == MAX_PID && strncmp((int8_t*)filename, (int8_t*)"exit", FIVE_BYTE) != SUCCESS){
        printf("Maximum number of process reached. Exit please\n");
        return FAILURE;
    }

    // dentry of the file
    dentry_t temp_dentry; 
    if(read_dentry_by_name(filename, &temp_dentry) == FAILURE)
        return FAILURE;

    // check if executable
    if(isExe(filename) == FAILURE) 
        return FAILURE;

    process_counter++;

    // find the available pid;
    int32_t this_pid = find_pid();
    // create the new page directory for the task
    page_directory[VIRTUAL_MEMORY_START] = (EIGHT_MB + (this_pid * FOUR_MB)) | PAGING_USER_ON; 
    flush_tlb();

    // find an open space for the process
    pcb_t* pcb_process = (pcb_t*)(EIGHT_MB - EIGHT_KB * (this_pid + 1));

    // initialize a pcb
    init_pcb(pcb_process, arg, this_pid);
    
    // find and save the entry point into the program
    uint8_t entry_point[FOUR_BYTE];
    read_data(temp_dentry.inode_num_, ENTRY_INDEX, entry_point, FOUR_BYTE);
    read_data(temp_dentry.inode_num_, 0, (uint8_t*)PROGRAM_IMG, inode_addr[temp_dentry.inode_num_].length_);

    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - this_pid * EIGHT_KB) - FOUR_BYTE;

    if(pcb_process->parent_pcb != NULL){
        asm volatile ("                 \n\
                movl %%ebp, %0          \n\
                movl %%esp, %1          \n\
                "
                :"=r"(pcb_process->parent_pcb->halt_ebp), "=r"(pcb_process->parent_pcb->halt_esp)
                :
        );
    }
    uint32_t entry_point_value = *((uint32_t*)entry_point);

    // switch to the user program
    context_switch(entry_point_value);

    asm volatile(
        "return:"
    );

    if(global_halt == 1){
        global_halt = 0;
        return EXCEPTION_256;
    }
    return 0;
}

/*
    read(int32_t fd, void* buf, int32_t nbytes)

    function:       read the buffer from the file descriptor
    input:          fd - the file descriptor we want to read from
                    buf - the buffer we want to read from
                    nbytes - number of bytes we want to read
    output:         0 on failure, number of bytes read on sucess
*/
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    //check for legal inputs
    if(nbytes < 0){
        return FAILURE;
    }
    if(fd < 0 || fd > END_OF_FDA){
        return FAILURE;
    }
    if(buf == NULL){
        return FAILURE;
    }
    if(terminal[curr_scheduler].pcb->file_descriptor[fd].flags_ == 0){
        return FAILURE;
    }
    //call the corresponding read function

    uint32_t num_read = terminal[curr_scheduler].pcb->file_descriptor[fd].pointer_.read_(fd,buf,nbytes);
    terminal[curr_scheduler].pcb->file_descriptor[fd].file_position_ += num_read;
    return num_read;
}

/*
    write(int32_t fd, const void* buf, int32_t nbytes)

    function:       writes the buffer to the file descriptor
    input:          fd - the file descriptor we want to write to
                    buf - the buffer we want to write
                    nbytes - number of bytes we want to write
    output:         0 on failure, 0 on success
*/
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    //check for legal inputs
    if(nbytes < 0){
        return FAILURE;
    }
    if(fd < 0 || fd > END_OF_FDA){
        return FAILURE;
    }
    if(buf == NULL){
        return FAILURE;
    }
    if(terminal[curr_scheduler].pcb->file_descriptor[fd].flags_ == 0){
        return FAILURE;
    }
    //call the corresponding write function
    return terminal[curr_scheduler].pcb->file_descriptor[fd].pointer_.write_(fd,buf,nbytes);
}

/*
    open(const uint8_t* filename)

    function:       finds an empty index in the file descriptor array
                    and opens the given file and stores the info
                    in the file descriptor
    input:          filename - the name of the file we want opened 
    output:         -1 on failure, 0 on success
*/
int32_t open(const uint8_t* filename){
    // check if inputs are legal
    if(filename == NULL || *filename == NULL){
        return FAILURE;
    }
    dentry_t temp_dentry;
    if(read_dentry_by_name(filename, &temp_dentry)!=0){
        return FAILURE;
    }
    //look for an open spot in file descriptor array
    int i;
    int fd = -1;
    for(i = BEGINING_FDA; i <= END_OF_FDA; i++){
        if(terminal[curr_scheduler].pcb->file_descriptor[i].flags_ == 0){
            fd = i;
            break;
        }
    }
    if(fd == -1){
        return FAILURE;
    }
    
    //check which type of file this is
    switch(temp_dentry.file_type_){
        //rtc
        case RTC_TYPE:
            terminal[curr_scheduler].pcb->file_descriptor[fd].pointer_ = rtc_fops;
            terminal[curr_scheduler].pcb->file_descriptor[fd].inode_ = NULL;
            terminal[curr_scheduler].pcb->file_descriptor[fd].file_position_ = 0;
            terminal[curr_scheduler].pcb->file_descriptor[fd].flags_ = 1;
            terminal[curr_scheduler].pcb->file_descriptor[fd].pointer_.open_(filename);
            return fd;
        //directory
        case DIR_TYPE:
            terminal[curr_scheduler].pcb->file_descriptor[fd].pointer_ = dir_fops;
            terminal[curr_scheduler].pcb->file_descriptor[fd].inode_ = NULL;
            terminal[curr_scheduler].pcb->file_descriptor[fd].file_position_ = 0;
            terminal[curr_scheduler].pcb->file_descriptor[fd].flags_ = 1;
            terminal[curr_scheduler].pcb->file_descriptor[fd].pointer_.open_(filename);
            return fd;
        //file
        case FILE_TYPE:
            terminal[curr_scheduler].pcb->file_descriptor[fd].pointer_ = file_fops;
            terminal[curr_scheduler].pcb->file_descriptor[fd].inode_ = temp_dentry.inode_num_;
            terminal[curr_scheduler].pcb->file_descriptor[fd].file_position_ = 0;
            terminal[curr_scheduler].pcb->file_descriptor[fd].flags_ = 1;
            terminal[curr_scheduler].pcb->file_descriptor[fd].pointer_.open_(filename);
            return fd;
        default:
            return FAILURE;
    }
}

/*
    close(int32_t fd)

    function:       closes the given file descriptor
    input:          fd - the file descriptor we want closed 
    output:         -1 on failure, 0 on success
    */
int32_t close(int32_t fd){
    //check for legal inputs
    if(fd < BEGINING_FDA || fd > END_OF_FDA){
        return FAILURE;
    }
    // clear the location of the file and close it
    if(terminal[curr_scheduler].pcb->file_descriptor[fd].flags_ == 0){
        return FAILURE;
    }
    // clear all of the attributes in the file
    terminal[curr_scheduler].pcb->file_descriptor[fd].inode_ = 0;
    terminal[curr_scheduler].pcb->file_descriptor[fd].file_position_ = 0;
    terminal[curr_scheduler].pcb->file_descriptor[fd].flags_ = 0;
    terminal[curr_scheduler].pcb->file_descriptor[fd].pointer_.close_(fd);
    return SUCCESS;
}

/*
    getargs(uint8_t* buf, int32_t nbytes)
    Copies the argument from the terminal[curr_scheduler].pcb into a buffer
    function:       get args
    input:          buf and nbytes
    output:         -1 on failure, 0 on success
*/
int32_t getargs(uint8_t* buf, int32_t nbytes){
    // parameters sanity check
    if(nbytes <= 0 || buf == NULL)
        return FAILURE;
    // if there is no arg
    if(strlen(terminal[curr_scheduler].pcb->argument) == 0)
        return FAILURE;    
    strncpy((int8_t*)buf, terminal[curr_scheduler].pcb->argument, nbytes);
    int i;
    // if there is no null-terminator return fail
    for(i=0; i<nbytes; i++){
        if(buf[i] == NULL_TERM) 
            return SUCCESS;
    }
    return FAILURE;
}

/*
    vidmap(uint8_t** screen_start)

    function:       maps video memory
    input:          double pointer on screen_start
    output:         -1 on failure, 0 on success
*/
int32_t vidmap(uint8_t** screen_start){
    cli();
    // Sanity check for the screen_start
    if(screen_start == NULL || (uint32_t)screen_start < LOWERBOUND_VID || (uint32_t)screen_start > UPPERBOUND_VID){ // bounds are 128 MB and 132 MB
        return FAILURE;
    }
    // call a function in paging.c that maps a 4MB chunk of memory from the virtual address to the first page of the page table
    // and sets the privilege level to user
    map_video_memory((uint32_t)MB136, (uint32_t)video_mem);    

    // if current scheduler and the current terminal are the same, map to the actual video memory
    // otherwise map to the teminal video memory
    if(curr_scheduler == current_terminal){
        page_table_video[video_mem] = (uint32_t)video_mem | vid_mem_flag;
    }
    else{
        page_table_video[video_mem] = (uint32_t)vid_page[curr_scheduler] | vid_mem_flag;
    }
    
    flush_tlb();
    // return the address
    *screen_start = (uint8_t*)(MB136);
    sti();
    return SUCCESS;
}

/*
    set_handler(int32_t signum, void* handler_address)

    function:       set handler
    input:          signum and handler_address
    output:         -1 on failure, 0 on success
*/
int32_t set_handler(int32_t signum, void* handler_address){
    return FAILURE;
}

/*
    sigreturn(void)

    function:       signal handling
    input:          no input
    output:         -1 on failure, 0 on success
*/
int32_t sigreturn(void){
    return FAILURE;
}

/*
    init_fops_table()

    function:       sets all the fops tables to their corresponding functions
    input:          none
    output:         none
*/
void init_fops_table(){
    stdin_fops.open_ = terminal_open;
    stdin_fops.close_ = terminal_close; 
    stdin_fops.write_ = invalid_write_op; 
    stdin_fops.read_ = terminal_read; 

    stdout_fops.open_ = terminal_open;
    stdout_fops.close_ = terminal_close; 
    stdout_fops.write_ = terminal_write; 
    stdout_fops.read_ = invalid_read_op; 

    rtc_fops.open_ = rtc_open;
    rtc_fops.close_ = rtc_close; 
    rtc_fops.write_ = rtc_write; 
    rtc_fops.read_ = rtc_read; 

    dir_fops.open_ = dir_open;
    dir_fops.close_ = dir_close; 
    dir_fops.write_ = dir_write; 
    dir_fops.read_ = dir_read; 

    file_fops.open_ = file_open;
    file_fops.close_ = file_close; 
    file_fops.write_ = file_write; 
    file_fops.read_ = file_read; 
}

/*
    parse(const uint8_t* command, uint8_t* filename, uint8_t* arg)

    function:       gets and stores the name of the file and the argument
    input:          command -- the command string
                    filename -- pointer to where the file name is stored
                    arg -- pointer to where the argument is stored
    output:         -1 on failure, 0 on success
*/
int32_t parse(const uint8_t* command, uint8_t* filename, uint8_t* arg){
    // command sanity check
    if (command == NULL) 
        return FAILURE;
    int i=0;
    int length=0;
    int argLen=0;
    arg[argLen] = NULL_TERM;

    // store the filename
    while (command[i] == SPACE) 
        i++;
    while (command[i] != SPACE && command[i] != NULL_TERM) {
        if(length == MAX_FILE_LEN) return FAILURE;
        filename[length] = command[i];
        i++;
        length++;
    }
    filename[length] = NULL_TERM;

    // store the argument
    if(command[i] == SPACE){
        while(command[i] == SPACE) i++;
        
        while(command[i] != NULL_TERM){
            if(argLen == MAX_ARG_LEN-length) return FAILURE;
            arg[argLen] = command[i];
            i++;
            argLen++;
        }
        arg[argLen] = NULL_TERM;
    }
    return SUCCESS;
}

/*
    isExe(uint32_t filename)

    function:       checks if the program is executable
    input:          filename - check if the program is executable
    output:         -1 on failure, 0 on success
*/
int32_t isExe(const uint8_t* filename){
    if(filename == NULL) 
        return FAILURE;
    dentry_t temp;
    // if there is no corresponding filename, return failure
    if(read_dentry_by_name(filename, &temp) == FAILURE) 
        return FAILURE;
    int8_t buf[EIGHT_BYTE];
    read_data(temp.inode_num_, 0, (uint8_t*)buf, FOUR_BYTE);
    // check for elf
    if(strncmp((int8_t*)buf, (int8_t*)magic_identifiers, FOUR_BYTE)) 
        return FAILURE;
    return SUCCESS;
}

/*
    init_pcb(pcb_t* pcb_process, uint8_t arg)

    function:       initalizes the pcb
    input:          pcb_process and the arguments for the pcb process
    output:         -1 on failure, 0 on success
*/
void init_pcb(pcb_t* pcb_process, uint8_t* arg, int32_t this_pid){
    int i;
    // if there is no processor running, run a new processor
    if(terminal[curr_scheduler].pcb == NULL){
        pcb_process->parent_pcb = NULL;
        pcb_process->pid = this_pid;
        pid[this_pid] = OCCUPIED;
        pcb_process->parent_pid = -1;
    }
    else{ // run another processor
        pcb_process->parent_pcb = terminal[curr_scheduler].pcb;
        pcb_process->pid = this_pid;
        pid[this_pid] = OCCUPIED;
        pcb_process->parent_pid = terminal[curr_scheduler].pcb->pid;
    }

    // store the argument
    pcb_process->argument = (int8_t*)arg;

    // replace the current pcb
    terminal[curr_scheduler].pcb = pcb_process;

    //initialize file descriptor values
    terminal[curr_scheduler].pcb->file_descriptor[0].pointer_ = stdin_fops;
    terminal[curr_scheduler].pcb->file_descriptor[0].inode_ = NULL;
    terminal[curr_scheduler].pcb->file_descriptor[0].file_position_ = NULL;
    terminal[curr_scheduler].pcb->file_descriptor[0].flags_ = 1;
    terminal[curr_scheduler].pcb->file_descriptor[1].pointer_ = stdout_fops;    
    terminal[curr_scheduler].pcb->file_descriptor[1].inode_ = NULL;
    terminal[curr_scheduler].pcb->file_descriptor[1].file_position_ = NULL;
    terminal[curr_scheduler].pcb->file_descriptor[1].flags_ = 1;

    // initialize the flags for files
    for(i = BEGINING_FDA; i < END_OF_FDA; i++){
        pcb_process->file_descriptor[i].flags_ = 0;
    }        
}

/*
    find_pid

    function:       find a valid pid, if all of the pid is unavailable, return failure
    input:          None
    output:         -1 on failure, pid value on success
*/
int32_t find_pid(){
    int i;
    for(i=0; i<=MAX_PID; i++){
        if(pid[i] == VACANT){
            return i;
        }
    }
    return FAILURE;
}
