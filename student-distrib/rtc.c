#include "rtc.h"

volatile int rtc_interrupt_occurred;

    /*
    rtc_init

    function:       initialize rtc with default frequency of 1024Hz
    input:          nonee
    output:         none
    side effect:    change value in register B
    magic numbers:  none
    the following code is retrieved from https://wiki.osdev.org/RTC
    */
void rtc_init(){
    outb(RTC_REG_B, RTC_REG_NUM);       // select register B, and disable NMI
    char prev = inb(RTC_READ_WRITE);    // read the current value of register B
    outb(RTC_REG_B, RTC_REG_NUM);       // set the index again (a read will reset the index to register D)
    outb(prev | BIT_6, RTC_READ_WRITE); // write the previous value ORed with 0x40. This turns on bit 6 of register B
    rtc_open(0);           //set intial frequency to 2Hz
    enable_irq(RTC_IRQ);   // enable rtc interrupt
}

    /*
    rtc_handler

    function:       handles rtc interrupts
    input:          none
    output:         none
    side effect:    change value in register C
    magic numbers:  none
    the following code is retrieved from https://wiki.osdev.org/RTC
    */
uint32_t rtc_handler(){
    send_eoi(RTC_IRQ);              // interrupt successfully finshed
    rtc_interrupt_occurred = UNLOCK;
    outb(RTC_REG_C, RTC_REG_NUM);   // select register C
    inb(RTC_READ_WRITE);            // just throw away contents
    return SUCCESS;
}

/* 
 * rtc_open
 *   DESCRIPTION: Opens the rtc and sets the freqency to 2 Hz
 *   INPUTS: filename -- the file to be opened, unused
 *   OUTPUTS: returns 0 after success
 */
int32_t rtc_open(const uint8_t* filename){
    outb(RTC_REG_A, RTC_REG_NUM);       // select register A, and disable NMI
    char prev = inb(RTC_READ_WRITE);    // get initial value of register A
    outb(RTC_REG_A, RTC_REG_NUM);       // reset index to A
    outb((prev & TOP_FOUR_BIT) | HZ_2, RTC_READ_WRITE);  // write only our rate to A. Note, rate is the bottom 4 bits.
    return SUCCESS;
}

/* 
 * rtc_read
 *   DESCRIPTION: Reads the contents of the rtc after an interrupt
 *   has been detected
 *   INPUTS: fd -- file descriptor, unused
 *           buf -- the file to read into, unused
 *           nbytes -- the number of bytes, unused
 *   OUTPUTS: returns 0 after success
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
    rtc_interrupt_occurred = LOCK;
    while(rtc_interrupt_occurred == LOCK){}     // loop until an interrupt is detected
    return SUCCESS;
}

/* 
 * rtc_write
 *   DESCRIPTION: Sets the frequency of the rtc depending on the 
 *   buffer
 *   INPUTS: fd -- file descriptor, unused
 *           buf -- the file to read into, unused
 *           nbytes -- the number of bytes, unused
 *   OUTPUTS: returns 0 after success
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
    // check that the number of bytes is 4 and that the buffer is not empty
    if((const uint32_t*)buf == 0 || nbytes != NUM_BYTES)
        return FAILURE;

    // save the register A's old value
    outb(RTC_REG_A, RTC_REG_NUM);
    unsigned char prev_a = inb(RTC_READ_WRITE);
    
    int8_t r;
    uint32_t value = *(const uint32_t*)buf;
    uint32_t power_of_two = (value)&(value-1);
    
    if(power_of_two != 0)
        return FAILURE;

    // set r depending on the value in the buffer
    switch(value)
    {
        case CASE_0:
            r = HZ_0;
            break;
        case CASE_2:
            r = HZ_2;
            break;
        case CASE_4:
            r = HZ_4;
            break;
        case CASE_8:
            r = HZ_8;
            break;
        case CASE_16:
            r = HZ_16;
            break;
        case CASE_32:
            r = HZ_32;
            break;
        case CASE_64:
            r = HZ_64;
            break;
        case CASE_128:
            r = HZ_128;
            break;
        case CASE_256:
			r = HZ_256;
            break;
        case CASE_512:
            r = HZ_512;
            break;
        case CASE_1024:
            r = HZ_1024;
            break;
        default:
            return FAILURE;
	}
    
    // set the rate in A to the correct value to set the frequency
    outb(RTC_REG_A, RTC_REG_NUM);
    outb((prev_a & UPPER_MASK) | (r & LOWER_MASK), RTC_READ_WRITE);
    return SUCCESS;
}

/* 
 * rtc_close
 *   DESCRIPTION: Closes the rtc
 *   INPUTS: fd -- the file descriptor
 *   OUTPUTS: returns 0 after success
 */
int32_t rtc_close(int32_t fd){
    return SUCCESS;
}
