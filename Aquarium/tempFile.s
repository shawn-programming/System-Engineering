      remove

      
        pushl %ebp      /* push old frame pointer */
        movl %esp, %ebp /* point to new frame */
        pushl %ebx      /* argument (location) */
        pushl %esi      /* current pointer */
        pushl %edi      /* found link's next */
                        /* ecx = found link's previous link */


        movl 8(%ebp), %ebx
        cmpw $2000, %ebx
        jnl invalid_


        cmpw %bx, LOCATION(%esi) /* check if we are removing the head */

        movl mp1_list_head, %ecx /* set previous pointer of removing link initially to the head */
remove_loop:
        cmpl $0, %esi
        jz invalid_

        cmpw %bx, LOCATION(%esi)
        jnz remove_next_node



        jnl free_struct
	ret


remove_next_node:
        movl NEXT(%esi), %esi
        jmp remove_loop