#include "file_sys.h"

static int directory_index = -1;

    /*
    file_sys_init(boot_block_t* start_addr)

    function:       set global address of the file system (boot_block, inode, data_block addr)
    input:          file_sys (file_system starting address)
    output:         none
    side effect:    initalizes global addresses
    magic numbers:  none
    */
void file_sys_init(boot_block_t* start_addr){
    boot_block_addr = (boot_block_t*)(start_addr);
    inode_addr = (inode_t*)(start_addr)+1;    // next block after the boot block
    uint32_t inode_num = boot_block_addr->file_stat_.inode_num_;
    data_block_addr = (data_block_t*)(inode_addr)+inode_num;  // the very first data block
}

    /*
    read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)

    function:       find the fname file and make dentry point to the corresponding dentry
    input:          fname = file name, dentry = pointer that points dentry
    output:         0 if successful, -1 if not
    side effect:    none
    magic numbers:  none
    */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    int i;
    // if fname is . dentry points directory dentry
    if(!strncmp((char*)fname, (char*)".", STRNCMP_32)){
        *dentry = boot_block_addr->dir_dentry_;
        return SUCCESS;
    }

    dentry_t curr_dentry;

    for(i=0; i<FILE_NUM; i++){
        curr_dentry = boot_block_addr->dentry_[i];
        int8_t temp[BYTE4+1];
        strncpy(temp, (const int8_t*)curr_dentry.file_name_, BYTE4);
        temp[BYTE4] = NULL_TERM;
        // compare name, if found, point the corresponding dentry and return 0
        if(!strncmp((char*)temp, (char*)fname, BYTE4+1)){
            *dentry = curr_dentry;
            return SUCCESS;
        }
    }

    // file not found, return -1
    return FAILURE;
}

    /*
    read_dentry_by_index (uint32_t index, dentry_t* dentry)

    function:       find the file with the index and make dentry point to the corresponding dentry
    input:          index = index of the file, dentry = pointer that points dentry
    output:         0 if successful, -1 if not
    side effect:    none
    magic numbers:  none
    */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){

    // sanity check
    if(index < 0 || index >= boot_block_addr->file_stat_.dir_entry_num_)
        return FAILURE;

    // file is directory file (".")
    if(index == 0){
        *dentry = boot_block_addr->dir_dentry_;
    }
    else{
        // dentry points to the file
        *dentry = boot_block_addr->dentry_[index-1]; // -1 because of directory file
    }

    return SUCCESS;
}

    /*
    read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)

    function:       read the file with the corresponding inode, store its data to the buffer
    input:          inode = inode index / offset = starting offsset / buf = buffer that stores data / length = # of bytes to be stored
    output:         # of bytes if successful, -1 if not
    side effect:    none
    magic numbers:  none
    */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    uint32_t max_data_block = boot_block_addr->file_stat_.data_block_num_;
    uint32_t max_inode = boot_block_addr->file_stat_.inode_num_;
    inode_t* inode_ptr = (inode_t*)(inode_addr)+inode;

    uint32_t file_size = inode_ptr->length_;

    if(inode >= max_inode || inode < 0 || buf == 0)  // if invalid inode or invalid buffer address
        return -1;

    uint32_t block_index = offset / KB4;     // block_index now contains the starting block index in the inode

    if(offset >= file_size)    // if offset is beyond the limit, return 0
        return 0;

    uint32_t curr_block_num = inode_ptr->data_block_[block_index]; // starting block number
    data_block_t* curr_block_ptr = data_block_addr+curr_block_num;
    uint32_t data_counter = 0;
    int i;
    offset -= block_index * KB4; // update the offset

    for(i=0; i<length; i++){
        if(data_counter == length || block_index == KB-INDEX_2) // if data are read for length amount or has reached the end
            return data_counter;                          // return number of read data
        if(max_data_block <= block_index){
            printf("invalid data block!\n");
            return data_counter;
        }
        if(offset == KB4){  // finished the current data block
            offset = 0;     // reset offset 
            block_index++;          
            curr_block_num = inode_ptr->data_block_[block_index];
            curr_block_ptr = data_block_addr+curr_block_num; // update current block
        }
        memcpy(&(buf[data_counter]), &(curr_block_ptr->data[offset]), 1);
        data_counter++;
        offset++;
    }
    return data_counter;
}

    /*
    file_open(const uint8_t* fname)

    function:       make fd points to the dentry with its file name as fname
    input:          fd = dentry pointer to be modified / fname = filename of the wanted fiile
    output:         0 if successful, -1 if not
    side effect:    none
    magic numbers:  none
    */

int32_t file_open(const uint8_t* fname){
    return SUCCESS;
}

    /*
    file_close(int32_t fd)

    function:       make fd points NULL pointer
    input:          fd = dentry pointer to be closed
    output:         0 if successful, -1 if not
    side effect:    none
    magic numbers:  none
    */
int32_t file_close(int32_t fd){
    return SUCCESS;
}
    /*
    file_write(int32_t fd, void const* buf, int32_t nbytes)

    function:       none
    input:          none
    output:         -1
    side effect:    none
    magic numbers:  none
    */
int32_t file_write(int32_t fd, void const* buf, int32_t nbytes){
    return FAILURE;
}

    /*
    file_read(int32_t fd, void* buf, int32_t nbytes)

    function:       read the file with the corresponding dentry, store its data to the buffer
    input:          offset = starting offsset / buf = buffer that stores data / length = # of bytes to be stored
    output:         # of bytes if successful, -1 if not
    side effect:    none
    magic numbers:  none
    */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes){
    uint32_t inode_index = terminal[curr_scheduler].pcb->file_descriptor[fd].inode_; 
    return read_data(inode_index, terminal[curr_scheduler].pcb->file_descriptor[fd].file_position_, (uint8_t*)buf, nbytes);
}

    /*
    dir_open(const uint8_t* fname)

    function:       make fd points to the directory dentry
    input:          filename
    output:         0 if successful, -1 if not
    side effect:    none
    magic numbers:  none
    */
int32_t dir_open(const uint8_t* fname){
    return SUCCESS;
}

    /*
    dir_close(int32_t fd)

    function:       make fd points to NULL ptrßß
    input:          fd = dentry pointer to be closed
    output:         0 if successful, -1 if not
    side effect:    none
    magic numbers:  none
    */
int32_t dir_close(int32_t fd){
    return SUCCESS;
}

    /*
    dir_write(int32_t fd, void const* buf, int32_t nbytes)

    function:       none
    input:          int32_t fd, void const* buf, int32_t nbytes
    output:         -1
    side effect:    none
    magic numbers:  none
    */
int32_t dir_write(int32_t fd, void const* buf, int32_t nbytes){
    return FAILURE;
}

    /*
    dir_read(int32_t fd, void* buf, int32_t nbytes)

    function:       read the filename of index-th dentry
    input:          int32_t fd, void* buf, int32_t nbytes
    output:         -1 if unsuccessful else pointer to the filename
    side effect:    none
    magic numbers:  none
    */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes){
    int num_file = (int)(boot_block_addr->file_stat_.dir_entry_num_);
    if(buf == NULL){
        return FAILURE;
    }
    // sanity check
    if(directory_index >= num_file-1){
        directory_index = -1;
        return SUCCESS;
    }
    // if index -1, return directory dentry's file name
    else if(directory_index == -1){
        strcpy((char*)buf, (char*)PERIOD);
    }
    // find the dentry, return file name
    else{
        strncpy((char*)buf, (char*)boot_block_addr->dentry_[directory_index].file_name_, BYTE4);
        memcpy(&(((char*)buf)[BYTE4]), NULL_CHAR, 1);
    }
    directory_index++;
    return strlen((char*)buf);    
}

    /*
    invalid_op(int32_t fd, void const* buf, int32_t nbytes)

    function:       returns failure for an invalid operation for stdin 
    input:          the inputs for write so there isn't an error trying to access this
    output:         -1
    side effect:    none
    magic numbers:  none
    */
int32_t invalid_write_op(int32_t fd, void const* buf, int32_t nbytes){
    return FAILURE;
}

    /*
    invalid_op(int32_t fd, void const* buf, int32_t nbytes)

    function:       returns failure for an invalid operation for  stdout
    input:          the inputs for read so there isn't an error trying to access this
    output:         -1
    side effect:    none
    magic numbers:  none
    */
int32_t invalid_read_op(int32_t fd, void* buf, int32_t nbytes){
    return FAILURE;
}
