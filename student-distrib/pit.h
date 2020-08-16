#ifndef _PIT_H
#define _PIT_H

#include "x86_desc.h"
#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "system_call.h"
#include "pcb.h"
#include "keyboard.h"

#define PIT_MODE    0x37  // 0x37 - pit mode is channel 0, lobyte/highbyte, mode 3
#define PIT_CMD     0x43  // 0x43 - command byte
#define PIT_FREQ    11932 // 11932 - pit frequency is ~1.1932 MHz, divide by 100 Hz
#define PIT_DATA    0x40  // 0x40 - channel 0 data port (read/write)
#define PIT_IRQ     0     // 0 - pit IRQ that is used to implement sleep
#define SHIFT_8     8     // Shift PIT Freq by 8 
#define MAX_TERMINAL 3    // system supports 3 terminals

void pit_init();
void pit_handler();
void scheduler();

#endif
