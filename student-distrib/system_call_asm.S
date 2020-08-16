#define ASM     1
#define SIZE_OF_ONE_MEMORY_LOCATION   4
#include "x86_desc.h"

#define USER_STACK 0x83FFFFC
#define IF_FLAG 0x200
#define UPPERBOUND 10
#define LOWERBOUND 1
#define NEGONE -1

.globl system_call_asm
.globl flush_tlb
.globl context_switch

/* jump table is indexed 1-10 with the functions we want to use */
jmp_table:
    .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
/*
    function:       system_call_asm -- calls the correct system call based
                    on what it is given and pushes the args into it
    input:          eax -- call number of which system call we want
                    edx -- arg 3
                    ecx -- arg 2
                    ebx -- arg 1
    output:         the value in eax or -1 on failure
*/
system_call_asm: 
/* check if the call number is invalid */
    cmpl $UPPERBOUND, %eax
    ja error
    cmpl $LOWERBOUND, %eax
    jb error
    
/* push flag and callee save registers */
    pushfl
    pushl %edi
    pushl %esi
    pushl %ebp

/* decrement eax to get the correct call number and push the args on the stack */
    dec %eax

    pushl %edx
    pushl %ecx
    pushl %ebx  

/* call the correct function */
    call *jmp_table(,%eax,SIZE_OF_ONE_MEMORY_LOCATION)

/* pop everything off the stack and return */

    popl %ebx
    popl %ecx
    popl %edx
    popl %ebp
    popl %esi
    popl %edi
       
    popfl
    iret

/* if invalid system call return -1 */
error:
    movl $NEGONE, %eax
    iret

/*
    function:       flush_tlb -- it moves the cr3 values into eax and moves eax into cr3, essential for paging
    input:          no inputs
    output:         no outputs
*/
flush_tlb:
    movl %cr3, %eax
    movl %eax, %cr3
    ret

/*
    function:       context_switch -- facilitates stack switching between kernel and user stack
    input:          the entry point of the program
    output:         no outputs
*/
context_switch: 
    /* save entry point */
    movl SIZE_OF_ONE_MEMORY_LOCATION(%esp), %ebx

    /* push USER DS */
    xorl %edx, %edx
    movw $USER_DS, %dx
    pushl %edx
    
    /* push USER STACK */
    movl  $USER_STACK, %edx
    pushl %edx

    /* push flags */
    pushfl

    /* set interrupt flag */
    xorl %edx, %edx
    popl  %edx
    orl   $IF_FLAG, %edx
    pushl %edx

    /* Push USER CS */
    xorl %edx, %edx
    movw $USER_CS, %dx
    pushl %edx

    /* push EIP, entrypoint */
    pushl %ebx

    /* return to user */
    iret
