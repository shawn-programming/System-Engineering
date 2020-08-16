#ifndef _SYSTEM_CALL_H
#define _SYSTEM_CALL_H

#include "x86_desc.h"
#include "types.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"
#include "file_sys.h"
#include "paging.h"
#include "pcb.h"
#include "system_call_asm.h"

#define VACANT -1
#define OCCUPIED 1
#define MAGIC_1 0x7F
#define MAGIC_2 0x45
#define MAGIC_3 0x4C
#define MAGIC_4 0x46
#define LOWERBOUND_VID 0x8000000
#define UPPERBOUND_VID 0x8400000
#define EIGHT_MB 0x800000
#define FOUR_MB  0x400000
#define EIGHT_KB 0x002000
#define FOUR_KB 0x001000
#define MB132   0x8400000
#define VIRTUAL_MEMORY_START  32
#define PAGING_USER_ON 0x87
#define FAILURE -1 
#define READ_FAILURE 0 
#define FOUR_BYTE 4
#define FIVE_BYTE 5
#define SIX_BYTE 6
#define EIGHT_BYTE 8
#define ENTRY_INDEX 24
#define PROGRAM_IMG 0x08048000
#define MAX_FILE_LEN 32
#define MAX_PID 5
#define MAX_ARG_LEN 128
#define MAX_ARG_NUM 32
#define END_OF_FDA 7
#define RTC_TYPE 0
#define DIR_TYPE 1
#define FILE_TYPE 2
#define BEGINING_FDA 2
#define SPACE ' '
#define NULL_TERM '\0'
#define EXCEPTION_256 256
#define MB136 0x8800000
#define vide_mem_addr 0xB8000
#define NEG_ONE -1

extern uint32_t global_halt;
extern uint8_t global_status;

extern uint32_t system_call_handler();
extern int32_t halt(uint8_t status);
extern int32_t execute(const uint8_t* command);
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open(const uint8_t* filename);
extern int32_t close(int32_t fd);
extern int32_t getargs(uint8_t* buf, int32_t nbytes);
extern int32_t vidmap(uint8_t** screen_start);
extern int32_t set_handler(int32_t signum, void* handler_address);
extern int32_t sigreturn(void);
extern int32_t parse(const uint8_t* command, uint8_t* filename, uint8_t* arg);
extern int32_t isExe(const uint8_t* filename);
extern void init_fops_table();
extern void init_pcb(pcb_t* pcb_process, uint8_t* arg, int32_t pid);
extern int32_t find_pid();

#endif
