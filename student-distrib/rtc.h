#ifndef _RTC_H
#define _RTC_H

#include "types.h"
#include "lib.h"
#include "i8259.h"

#define RTC_REG_NUM     0x70        // port used to specify an index or register number
#define RTC_READ_WRITE  0x71     // port used to read or write from/to that byte of CMOS configuration space
#define RTC_REG_A       0x8A          // RTC Status Register A
#define RTC_REG_B       0x8B          // RTC Status Register B
#define RTC_REG_C       0x8C          // RTC Status Register C
#define BIT_6           0x40              // Enable bit 6
#define TOP_FOUR_BIT    0xF0       // Enable first 4 bits and disable last 4 bits
#define RTC_IRQ         0x08            // IRQ number of RTC
#define HZ_0			0x00
#define HZ_2			0x0F
#define HZ_4			0x0E
#define HZ_8			0x0D
#define HZ_16			0x0C
#define HZ_32			0x0B
#define HZ_64			0x0A
#define HZ_128			0x09
#define HZ_256			0x08
#define HZ_512			0x07
#define HZ_1024			0x06
#define UPPER_MASK      0xF0
#define LOWER_MASK      0x0F
#define NUM_BYTES       4
#define CASE_0			0
#define CASE_2			2
#define CASE_4			4
#define CASE_8			8
#define CASE_16			16
#define CASE_32			32
#define CASE_64			64
#define CASE_128		128
#define CASE_256		256
#define CASE_512		512
#define CASE_1024		1024
#define LOCK            1
#define UNLOCK          0
#define SUCCESS         0
#define FAILURE         -1

extern void rtc_init();
extern uint32_t rtc_handler();
extern int32_t rtc_open(const uint8_t* filename);
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t rtc_close(int32_t fd);

#endif
