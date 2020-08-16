#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "file_sys.h"
#include "keyboard.h"
#include "rtc.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

// =========================== idt tests =============================
/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;
	
	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* IDT Test - Division by Zero
 * Expection: raise a division by zero exception
 * divide a integer by 0
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage:
 * Files: idt.c/h
 */
int div_zero_test(){
	TEST_HEADER;
	int a = 0;
	int b = 1;
	int c;
	c = b/a;
	return PASS;
}
// ======================================================================





// =========================== paging tests =============================
/* dereference_null_test - derefernce a null pointer
 * Expection: raise a page fault
 * dereference a null pointer
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage:
 * Files: paging.c,h
 */
int dereference_null_test(){
	TEST_HEADER;
	uint32_t* a = (uint32_t*)0x0;
	uint32_t b = *a;
	b++;	// needed to not have a warning
	return PASS;
}
/* dereference_invalid_pointer_test - derefernce an invalid pointer
 * Expection: raise a page fault
 * dereference a disabled pointer
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage:
 * Files: paging.c,h
 */

int dereference_invalid_pointer_test(){
	TEST_HEADER;
	uint32_t* a = (uint32_t*)0x33333333;  //0x33333333 is an invalid pointer (disabled)
	uint32_t b = *a;
	b++;  // needed to not have a warning
	return PASS;
}


/* dereference_vid_mem_test - check if video memory page is successfully loaded
 * Expection: PASS
 * dereference a video memory addreses
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage:
 * Files: paging.c,h
 */
int dereference_vid_mem_test(){
	TEST_HEADER;	
	uint32_t* a = (uint32_t*)0xb8000;   //0xb8000 is the memory address of video memory
	uint32_t b = *a;
	b++;	// needed to not have a warning
	return PASS;
}

/* dereference_kernel - check if kernel page is successfully loaded
 * Expection: PASS
 * dereference a kernel addreses
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage:
 * Files: paging.c,h
 */
int dereference_kernel(){
	TEST_HEADER;	
	uint32_t* a = (uint32_t*)0x00400000;   //0x00400000 is the memory address of video memory
	uint32_t b = *a;
	b++;	// needed to not have a warning
	return PASS;
}
// ======================================================================


/* Checkpoint 2 tests */
/* terminal_write_test - check the functionality of terminal_write
 * Expection: print asdf on a new line, PASS
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_write
 * Files: keyboard.c,h
 */
int terminal_write_test(){
	TEST_HEADER;	
	char terminal_input[128];
	strcpy(terminal_input, "asdf\n");
	terminal_write(0, terminal_input, 5);
	return PASS;
}

/* terminal_write_test - check the functionality of terminal_write for garbage input
 * Expection: print asdf with garbage, PASS
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_write
 * Files: keyboard.c,h
 */
int terminal_write_garbage_test(){
	TEST_HEADER;	
	char terminal_input[128];
	strcpy(terminal_input, "asdf");
	terminal_write(0, terminal_input, 8);
	return PASS;
}

/* terminal_read_test - check the functionality of terminal_read
 * Expection: repeat text on screen after presseing enter
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_read, terminal_wrte
 * Files: keyboard.c,h
 */
int terminal_read_test(){
	TEST_HEADER;	
	while(1){
		char terminal_input[128];
		terminal_write(0, terminal_input, terminal_read(0, terminal_input, 128));
	}
	return PASS;
}

/* rtc_driver_test - check the functionality of rtc_driver
 * Expection: at different frequency, print 1 on the screen
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: rtc_open, rtc_write, rtc_read, rtc_close
 * Files: rtc.c,h
 */
int rtc_driver_test(){
	TEST_HEADER;
	int i;
	uint32_t frequency = 2;
	while(frequency <= 1024){
		rtc_write(0, &frequency, 4);
		for (i = 0; i < 5; i++){
			rtc_read(0, 0, 0);
			terminal_write(0, "1", 1);
		}
		frequency += 1;
	}
	rtc_open(0);
	for (i = 0; i < 20; i++){
		rtc_read(0, 0, 0);
		terminal_write(0, "1", 1);
	}
	terminal_write(0, "\n", 1);
	rtc_close(0);
	return PASS;
}

// =========================== file_system tests =============================
/* dir_read_test - check the functionality of dir_read
 * Expection: print out the list of files, PASS
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: dir_read
 * Files: file_sys.c,h
 */
// int dir_read_test(){
// 	TEST_HEADER;
	
// 	uint32_t return_val; // pointer storing the filename
// 	int index = -1;
// 	while(1){
// 		// store the file name at return_val
// 		return_val = dir_read(0, 0, 0, index);
// 		index++;
// 		if(return_val == -1){
// 			printf("Reached the end. \n");
// 			return PASS;
// 		}
// 		printf("File \"%s\" is read! \n", (char*)return_val);
// 	}
// }

/* read_dentry_by_name_test()
 * read dentry with the filename as fname
 * Expection: PASS
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: read_dentry_by_name
 * Files: file_sys.c,h
 */
// int read_dentry_by_name_test(){
// 	TEST_HEADER;
// 	dentry_t temp;
// 	char* fname = "fish\0";
// 	// find the dentry "fish"
// 	read_dentry_by_name((uint8_t*)fname, &temp);
// 	// compare if they match
// 	if(strncmp(fname, (char*)temp.file_name_, BYTE4) == 0){
// 		printf("%s is successfully read\n", (char*)temp.file_name_);
// 		return PASS;
// 	}
	
// 	printf("Test failed, a file %s is read\n", (char*)temp.file_name_);

// 	return FAIL;
// }


/* read_dentry_by_name_failure_test()
 * try to read a filename that is too long
 * Expection: FAIL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: read_dentry_by_name
 * Files: file_sys.c,h
 */
// int read_dentry_by_name_failure_test(){
// 	TEST_HEADER;
// 	dentry_t temp;
// 	char* fname = "verylargetextwithverylongname.txt";
// 	// find the dentry "fish"
// 	read_dentry_by_name((uint8_t*)fname, &temp);
// 	// compare if they match
// 	if(strncmp(fname, (char*)temp.file_name_, BYTE4) == 0){
// 		printf("%s is successfully read\n", (char*)temp.file_name_);
// 		return PASS;
// 	}
	
// 	printf("Test failed, a file %s is read\n", (char*)temp.file_name_);

// 	return FAIL;
// }

/* read_dentry_by_index_test()
 * read dentry at a certain index
 * Expection: PASS
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: read_dentry_by_index
 * Files: file_sys.c,h
 */
// int read_dentry_by_index_test(){
// 	TEST_HEADER;
// 	dentry_t temp;
// 	char* fname = ".\0";
// 	// find 0th dentry
// 	read_dentry_by_index(0, &temp);
// 	// check if they match
// 	if(strncmp(fname, (char*)temp.file_name_, BYTE4) == 0){
// 		printf("%s is successfully read\n", (char*)temp.file_name_);
// 		return PASS;
// 	}

// 	printf("Test failed, a file %s is read\n", (char*)temp.file_name_);

// 	return FAIL;
// }

	/*
    print_buffer (uint8_t* buffer, uint32_t size)

    function:       helper function that prints out the data in buffer
    input:          buffer = buffer to be read, size = # of bytes to be read
    output:         none
    side effect:    print out the data in the buffer
    magic numbers:  none
    */
// void print_buffer(uint8_t* buffer, uint32_t size){
// 	uint32_t i=0;
// 	for(i=0; i<size; i++){
// 		if(buffer[i] == 0) // if null, ignore
// 			continue;
// 		putc(buffer[i]);
// 	}
// }

/* read_data_test()
 * read data at certain dentry
 * Expection: PASS
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: read_dentry_by_index, read_data
 * Files: file_sys.c,h
 */
// int read_data_test(){
// 	TEST_HEADER;
// 	dentry_t temp;
// 	uint32_t length;

// 	read_dentry_by_index(13, &temp); // read dentry at certain index (12 is the index of an arbitrary file)

// 	length = (inode_addr+temp.inode_num_)->length_; // length of the file

// 	printf("inode number: %u\n", temp.inode_num_);
// 	printf("file name: %s\n", temp.file_name_);
// 	printf("file length: %u\n", length);

	
// 	uint8_t buffer[length];
// 	read_data(temp.inode_num_, 0, buffer, length); // read data with certain inode
// 	print_buffer(buffer, length); // print out the buffer

// 	return PASS;
// }

/* file_test()
 * open and close the directory and a fileßß
 * Expection: PASS
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: dir_open, dir_close, file_open, file_close
 * Files: file_sys.c,h
 */
// int file_test(){
// 	TEST_HEADER;

// 	dentry_t temp;
// 	dir_open(0, (int32_t)&temp);
// 	dir_close((int32_t)&temp);
// 	if(file_open((uint8_t*)"fish", (int32_t)&temp)==0){
// 		return PASS;
// 	}
// 	file_close((int32_t)&temp);
// 	return FAIL;
// }

/* file_read_test()
 * read a file with a certain name
 * Expection: PASS
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: file_open, file_read
 * Files: file_sys.c,h
 */
// int file_read_test(){
// 	TEST_HEADER;
// 	int32_t RESULT;
// 	dentry_t temp;
// 	file_open((uint8_t*)"frame0.txt", (int32_t)&temp); // open a file named "frame0.text"

// 	int32_t length = (inode_addr+temp.inode_num_)->length_; // length of the file
// 	int8_t buf[length];
// 	RESULT = file_read((int32_t)&temp, (void*)buf, (int32_t)length); // read the file
// 	print_buffer((uint8_t*)buf, length); // print the file data

// 	return RESULT == -1 ? FAIL : PASS;
// }
// ======================================================================

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// ==========================checkpoint 1 ==========================
	// TEST_OUTPUT("idt_test", idt_test());  													// expectation - PASS
	// TEST_OUTPUT("div_by_zero_test", div_zero_test()); 										// expectation - div by zero exception
	// TEST_OUTPUT("dereference null pointer test", dereference_null_test()); 					// expectation - page fault excetopn
	// TEST_OUTPUT("dereference invalid pointer test", dereference_invalid_pointer_test()); 	// expectation - page fault excetopn
	// TEST_OUTPUT("dereference video memory test", dereference_vid_mem_test()); 				// expectation - PASS
	// TEST_OUTPUT("dereference kernel test", dereference_kernel()); 							// expectation - PASS

	// ==========================checkpoint 2 ==========================
	// TEST_OUTPUT("terminal write test", terminal_write_test()); 							    // expectation - PASS
	// TEST_OUTPUT("terminal write garbage test", terminal_write_garbage_test()); 		        // expectation - PASS
	// TEST_OUTPUT("terminal read test", terminal_read_test()); 							    // expectation - PASS
	// TEST_OUTPUT("rtc driver test", rtc_driver_test()); 							            // expectation - PASS
	// TEST_OUTPUT("dir_read_test", dir_read_test());											// expectation - PASS
	// TEST_OUTPUT("read_dentry_by_name_test", read_dentry_by_name_test());						// expectation - PASS
	// TEST_OUTPUT("read_dentry_by_name_failure_test", read_dentry_by_name_failure_test());		// expectation - FAIL
	// TEST_OUTPUT("read_dentry_by_index_test", read_dentry_by_index_test());					// expectation - PASS
	// TEST_OUTPUT("read_data_test", read_data_test());											// expectation - PASS
	// TEST_OUTPUT("file_test", file_test());													// expectation - PASS
	// TEST_OUTPUT("file_read_test", file_read_test());											// expectation - PASS
	// launch your tests here
}
