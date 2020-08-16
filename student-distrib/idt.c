#include "idt.h"

/*
    idt_init
	This source helped understand the IDT: https://wiki.osdev.org/Interrupt_Descriptor_Table

    function:       this function initaliazes the IDT and sets the IDT entries for 0-19 exceptions and handles keyboard, rtc and system calls.
    input:          none
    output:         none
    side effect:    initalizes idt and sets the entries in the IDT
    magic numbers:  none
    */
void idt_init() {
	int i;
	for (i = 0; i < NUM_VEC; i++){
		// Enable present
		idt[i].present = 1;
		// If system call, set dpl to 3, all others to 0
		idt[i].dpl = (i == SYSTEM_CALL_VECTOR) ? SYSTEM_CALL_GATE : 0;
		// Set size to 32 bits
		idt[i].size = 1;
		// Handler runs in kernal space
		idt[i].seg_selector = KERNEL_CS;
		// Set unused bits to fixed values
		idt[i].reserved0 = 0;
		idt[i].reserved1 = 1;
		idt[i].reserved2 = 1;
		// Set reserved3 depending on what type of gate is being set
		idt[i].reserved3 = (i >= INTERRUPT_START_VECTOR && i <= INTERRUPT_END_VECTOR) ? 0 : 1;
		idt[i].reserved4 = 0;
	}

	SET_IDT_ENTRY(idt[0], de_asm); //gate 1
	SET_IDT_ENTRY(idt[1], db_asm); //gate 2
	SET_IDT_ENTRY(idt[2], nmi_asm); //gate 3
	SET_IDT_ENTRY(idt[3], bp_asm); //gate 4
	SET_IDT_ENTRY(idt[4], of_asm); //gate 5
	SET_IDT_ENTRY(idt[5], br_asm); //gate 6
	SET_IDT_ENTRY(idt[6], ud_asm); //gate 7
	SET_IDT_ENTRY(idt[7], nm_asm); //gate 8
	SET_IDT_ENTRY(idt[8], df_asm); //gate 9 
	SET_IDT_ENTRY(idt[9], cso_asm); //gate 10
	SET_IDT_ENTRY(idt[10], ts_asm); //gate 11
	SET_IDT_ENTRY(idt[11], np_asm); //gate 12
	SET_IDT_ENTRY(idt[12], ss_asm); //gate 13
	SET_IDT_ENTRY(idt[13], gp_asm); //gate 14
	SET_IDT_ENTRY(idt[14], pf_asm); //gate 15
	SET_IDT_ENTRY(idt[16], mf_asm); //gate 16
	SET_IDT_ENTRY(idt[17], ac_asm); //gate 17 
	SET_IDT_ENTRY(idt[18], mc_asm); //gate 18
	SET_IDT_ENTRY(idt[19], xf_asm); //gate 19
	SET_IDT_ENTRY(idt[KEYBOARD_VECTOR], keyboard_asm);
	SET_IDT_ENTRY(idt[RTC_VECTOR], rtc_asm);
	SET_IDT_ENTRY(idt[SYSTEM_CALL_VECTOR], system_call_asm);
	SET_IDT_ENTRY(idt[PIT_VECTOR], pit_asm);
}
