#include "exception_handler.h"

/*
    exception_macro	
    function:       this function creates macros for the following exceptions and prints the exception name and leaves the kernel in an infinite loop.
    input:          Mnemonic of the exception name from the Intel ISA and the error string to be printed
    output:         none
    side effect:    creates macros that print the exception name
    magic numbers:  none
*/
#define EXCEPTION(exception_macro, error_msg)				\
void exception_macro(){								    \
	printf("%s\n", #error_msg);					\
    global_halt = 1;                            \
	halt(1);                                    \
    while(1);                                   \
}                                               \

EXCEPTION(DE_exception, "Divide Error");
EXCEPTION(DB_exception, "RESERVED");
EXCEPTION(NMI_exception, "NMI Interrupt");
EXCEPTION(BP_exception, "Breakpoint Exception");
EXCEPTION(OF_exception, "Overflow Exception");
EXCEPTION(BR_exception, "BOUND Range Exceeded");
EXCEPTION(UD_exception, "Invalid Opcode");
EXCEPTION(NM_exception, "Device Not Available");
EXCEPTION(DF_exception, "Double Fault");
EXCEPTION(CSO_exception, "Coprocessor Segment Overrun");
EXCEPTION(TS_exception, "Invalid TSS");
EXCEPTION(NP_exception, "Segment Not Present");
EXCEPTION(SS_exception, "Stack-Segment Fault");
EXCEPTION(GP_exception, "General Protection");
EXCEPTION(PF_exception, "Page Fault");
EXCEPTION(MF_exception, "x87 FPU Floating-Point error");
EXCEPTION(AC_exception, "Alignment Check");
EXCEPTION(MC_exception, "Machine Check");
EXCEPTION(XF_exception, "SIMD Floating-Point Exception");
