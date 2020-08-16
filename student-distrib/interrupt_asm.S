.globl keyboard_asm
.globl rtc_asm
.globl pit_asm

    /*
    function:       keyboard_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls keyboard_handler, pops the registers and uses iret
    */
keyboard_asm:
    pushal
    call keyboard_handler
    popal
    iret

    /*
    function:       rtc_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls rtc_handler, pops the registers and uses iret
    */
rtc_asm:
    pushal
    call rtc_handler
    popal
    iret

    /*
    function:       pit_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls pit_handler, pops the registers and uses iret
    */
pit_asm:
    pushal
    call pit_handler
    popal
    iret
