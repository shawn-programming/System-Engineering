#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"
#include "types.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "interrupt_asm.h"
#include "system_call_asm.h"
#include "exception_asm.h"

#define SYSTEM_CALL_GATE          0x03
#define INTERRUPT_START_VECTOR    0x20
#define KEYBOARD_VECTOR		      0x21
#define RTC_VECTOR		          0x28
#define INTERRUPT_END_VECTOR      0x2F
#define SYSTEM_CALL_VECTOR        0x80
#define PIT_VECTOR                0x20

extern void idt_init();

#endif
