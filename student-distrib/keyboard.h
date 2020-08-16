#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "x86_desc.h"
#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "system_call.h"
#include "paging.h"
#include "pcb.h"
#include "pit.h"

// we used https://en.wikipedia.org/wiki/Interrupt_request_%28PC_architecture%29 as help for keyboard to PIC
#define KEYBOARD_BUFFER_SIZE        0x7F // the max size of the keyboard buffer without \n
#define KEYBOARD_CONTROLLER_PORT    0x60 // the keyboard sends data to port 0x60
#define KEYBOARD_CONTROLLER_IRQ     0x01 // the keyboard is connected to the PIC at IRQ 1 
#define SUCCESS             0
#define FAILURE             -1
#define ON                  1
#define OFF                 0
#define LOCK                1
#define UNLOCK              0
#define ZERO_BYTES          0
#define L_KEY               0x26
#define L_SHIFT_RELEASE     0xAA
#define R_SHIFT_RELEASE     0xB6
#define L_SHIFT_PRESSED     0x2A
#define R_SHIFT_PRESSED     0x36
#define CAPSLOCK_PRESSED    0x3A
#define CAPSLOCK_RELEASE    0xBA
#define CAPSLOCK_STATE_0    0
#define CAPSLOCK_STATE_1    1
#define CAPSLOCK_STATE_2    2
#define CAPSLOCK_STATE_3    3
#define CONTROL_PRESSED     0x1D
#define CONTROL_RELEASE     0x9D
#define BACKSPACE_PRESSED   0x0E
#define TAB_PRESSED         0x0F
#define TAB_LENGTH          4
#define NUM_ROWS            25
#define KEYBUFFER_LENGTH    128
#define SPACE               ' '
#define NEWLINE             '\n'
#define NULL_TERM           '\0'
#define C_KEY               0x2E
#define CTRL_CLEAR_VAL      2
#define F1_KEY              0x3B
#define F2_KEY              0x3C
#define F3_KEY              0x3D
#define LEFT_ALT            0x38
#define LEFT_ALT_RELEASE    0xB8
#define MAX_TERMINAL    3
#define TERM_0          0
#define TERM_1    1
#define TERM_2   2
#define CURSOR_START 0

uint8_t term_paging;

extern void keyboard_init(); /* Initialize the keyboard */
extern uint32_t keyboard_handler(); /* this displays the correct character to the screen when the keyboard sends an interrupt */
extern int32_t terminal_open(const uint8_t* filename);
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t terminal_close(int32_t fd);
extern int32_t terminal_switch(int32_t new_terminal);
extern void swap_kb_cursor(int32_t new_terminal);
extern void swap_vid_mem(int32_t new_terminal);
extern void terminal_init();

#endif
