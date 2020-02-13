
.data					# section declaration

        # Useful offset constants for accessing members of a 
        # struct mp1_blink_struct structure
        LOCATION   = 0    
        ON_CHAR    = 2
        OFF_CHAR   = 3 
        ON_LENGTH  = 4
        OFF_LENGTH = 6
        COUNTDOWN  = 8
        STATUS     = 10
        NEXT       = 12

        STRUCT_SIZE = 16

# Pointer to head of list (initialized to NULL)
mp1_list_head:
        .long   0

.text					# section declaration

# Export the function symbol names

.global mp1_rtc_tasklet
.global mp1_ioctl

# void mp1_poke(void);
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %cl  - The byte you wish to write  //on_char
#            %eax - Offset from the start of video memory that you wish
#                   to write to  //location
#   Outputs: Text-mode video screen is written to at location %eax with
#            the byte in %cl
# Registers: Clobbers EDX
mp1_poke:
        
	movl    vmem_base_addr(,1),%edx
	movb    %cl,(%edx,%eax,1)
	ret

/*
args: no argumemt
returns: void
side effect: clobbers eax
functionality: decrements countdown of each struct, if countdown == 0, 
               check status, change character to the other, put the changed character to the mp1_poke
*/      
/* void mp1 rtc tasklet (unsigned long); */
mp1_rtc_tasklet: 
        pushl %ebp      /* push old frame pointer */
        movl %esp, %ebp /* point to new frame */
        pushl %ebx      /* current node */
        pushl %esi      /* temp */
        pushl %edi

        movl mp1_list_head, %ebx /* ebx is the current pointer */

check_this_node:
        cmpl $0, %ebx
        jz list_ended             /* current point = NULL */

        addw $-1, COUNTDOWN(%ebx) /* decrements countdown */
        cmpw $0, COUNTDOWN(%ebx)  /* check countdown reached the zero */
        ja next_node

        cmpw $0, STATUS(%ebx)     /* if status == 0, push ON_CHAR to the mp1_poke*/
        jz status_is_off
        addw $-1, STATUS(%ebx)    /* STATUS upadated to 0 */
        movb OFF_CHAR(%ebx), %cl  /* if status != 0, push OFF_CHAR to the mp1_poke*/
        jmp do_poke
status_is_off:
        addw $1, STATUS(%ebx)     /* STATUS updated to  1 */
        movb ON_CHAR(%ebx), %cl

do_poke:
        xorl %eax, %eax
        movw LOCATION(%ebx), %ax
        shlw $1, %ax
        call mp1_poke             /* put oppsite character with mp1_poke */
  
        cmpw $0, STATUS(%ebx)     /* if status == 0, update countdown with OFF_CHAR */
        jz COUNTDOWN_is_OFF_LENGTH

        xorl %ecx, %ecx           /* update countdown to ON_LENGTH */
        movw ON_LENGTH(%ebx), %cx
        movw %cx, COUNTDOWN(%ebx) 

        jmp next_node

COUNTDOWN_is_OFF_LENGTH:
        xorl %ecx, %ecx  
        movw OFF_LENGTH(%ebx), %cx/* update countdown to OFF_LENGTH */
        movw %cx, COUNTDOWN(%ebx)

next_node:
        movl NEXT(%ebx), %ebx     /* update current node */
        jmp check_this_node        

list_ended:
        popl %edi
        popl %esi
        popl %ebx
        leave 
	ret

/*
args: cmd = offset for jump_table (stored in edx) / arg = argument for future functions
returns: depends on called functions
side effect: clobbers eax
functionality: call functions regarding the cmd value
*/
mp1_ioctl:
        movl 8(%esp), %edx    
        cmpl $3, %edx
        ja invalid_cmd
        jmp *jump_table(, %edx, 4)  /* if cmd <= 3 go to the corresponding function*/

invalid_cmd:
        movl $-1, %eax
        ret

/*
args: arg = user level pointer to a structure (saved in edi)
returns: 0 if everything suceeds else -1
side effect: clobbers eax
functionality: allocate new memory, copy the given structure to the allocated memory
               update its countdown, attack it to the head of the linked list and poke
registers: ebx = new struct pointer, esi = # of unavailable bytes after mp1_copy_user, edi = arg
*/
mp1_ioctl_add:
        pushl %ebp              /* push old frame pointer */
        movl %esp, %ebp         /* point to new frame */
        pushl %ebx
        pushl %esi      
        pushl %edi

        movl 8(%ebp), %edi      /* edi = arg */
        cmpl $0, %edi
        jz error_founded        /* arg == NULL*/


        /* allocation memory and store its pointer on ebx*/
        pushl %eax
        pushl %ecx
        pushl %edx
        pushl $STRUCT_SIZE
        call mp1_malloc
        addl $4, %esp
        movl %eax, %ebx         /*ebx = new struct pointer*/
        popl %edx
        popl %ecx
        popl %eax

        cmpl $0, %ebx
        jz error_founded        /* memory allocation failed*/

        /* copy the given user-level structure to the allocated memory */
        pushl %eax
        pushl %ecx
        pushl %edx         
        pushl $STRUCT_SIZE
        pushl 8(%ebp)
        pushl %ebx   
        call mp1_copy_from_user
        addl $12, %esp
        movl %eax, %esi         /* esi = # of unavailable bytes */
        popl %edx
        popl %ecx
        popl %eax

        /* if mp1_copy_from_user returns other than 0 or Location is not from 0 to 1999, free the struct*/
        cmpl $0, %esi
        ja free_struct
        cmpw $2000, LOCATION(%ebx)  /* check if location is greater than or equal to 2000 (invalid memeory address) */
        jnb free_struct

        /* update countdown with on_length */
        movw ON_LENGTH(%ebx), %dx
        movw %dx, COUNTDOWN(%ebx)

        movw $1, STATUS(%ebx)    /* status = 1*/

        /* update the head with the created struct */
        movl mp1_list_head, %edx
        movl %edx, NEXT(%ebx)
        movl %ebx, mp1_list_head

        /* call poke */
        movb ON_CHAR(%ebx), %cl
        xorl %eax, %eax
        movw LOCATION(%ebx), %ax
        shlw $1, %ax
        call mp1_poke


        popl %edi
        popl %esi
        popl %ebx

        movl $0, %eax
        leave
	ret

/* free the allocated structure */
free_struct:
        pushl %eax
        pushl %ecx
        pushl %edx 
        pushl %ebx
        call mp1_free
        add $4, %esp
        popl %edx
        popl %ecx
        popl %eax
        
/* return -1 */
error_founded:
        popl %edi
        popl %esi
        popl %ebx

        movl $-1, %eax
        leave
        ret
/*
args: ebx = head / esi = argument pointer
returns: returns 0 if can't find the corresponding struct
         returns 1 if successfully found
side effect: clobbers eax
functionality:  compare the argument with each of the link,
                if found, disconnect it from the list
                free the disconnected and return 0.
                if can't find, return -1
registers: ebx = new struct pointer / esi = pointer of desired struct
           edx = location arguemnt  / edi = pointer of next of desired struct
*/
mp1_ioctl_remove:       /*int mp1 ioctl remove(unsigned long arg)*/
        pushl %ebp      /* push old frame pointer */
        movl %esp, %ebp /* point to new frame */
        pushl %ebx      /* head */
        pushl %esi      /* pointer of desired struct */
        pushl %edi      /* pointer of the next of desired struct */

        movl mp1_list_head, %ebx        /* ebx = head */
        cmpl $0, %ebx
        jz invalid_value                /* null = invalid */

        xorl %edx, %edx
        movl 8(%ebp), %edx              /* edx = argument location */
        cmpw $2000, %dx                 /* check if location is greater than or equal to 2000 (invalid memeory address) */
        jnb invalid_value               

        cmpw LOCATION(%ebx), %dx        /* check whether the head is the desired struct */
        jnz remove_search

        /* remove head */
delete_head:
        movl NEXT(%ebx), %edi
        movl %edi, mp1_list_head
        movl %ebx, %esi
        jmp remove_free

remove_search:
        pushl %edx
        call search                      /* find the previous pointer of the structure with desired location */
        addl $4, %esp                    

        cmpl $0, %eax
        jz invalid_value                 /* null = invalid */

        movl NEXT(%eax), %esi            /* disconnect the desired struct from the list */
        movl NEXT(%esi), %edi
        movl %edi, NEXT(%eax)

/* free allocated memory located in esi */
remove_free:
        pushl %eax
        pushl %ecx
        pushl %edx 
        pushl %esi         
        call mp1_free
        add $4, %esp
        popl %edx
        popl %ecx
        popl %eax

        pop %edi
        pop %esi
        pop %ebx
        movl $0, %eax
        leave
        ret
/*
args: pointer that stores the wanted location and where the copied values will be stored
returns: if head is null or can't find desired struct, return -1
         if successfully find the desired struct, return 0
side effect: clobbers eax
functionality:  compares arg's location with the each of the link,
                if it finds a link with the same location, copy that struct into the arg
registers: ebx = temp that holds head / esi = argument pointer
           edx = the last two digits stores the location 
*/
mp1_ioctl_find:         /*int mp1 ioctl find(unsigned long arg)*/
        pushl %ebp      /* push old frame pointer */
        movl %esp, %ebp /* point to new frame */
        pushl %ebx      /* head */
        pushl %esi      /* argument pointer */
        pushl %edi

        movl mp1_list_head, %ebx /* ebx = head */
        cmpl $0, %ebx
        jz invalid_value         /* null = invalid */

        movl 8(%ebp), %esi
        xor %edx, %edx
        movw LOCATION(%esi), %dx /* dx stores argument's location */
        cmpw $2000, %dx          /* check if location is greater than or equal to 2000 (invalid memeory address) */
        jnb invalid_value       /* null = invalid */

        cmpw %dx, LOCATION(%ebx) /* check if the head is the desired struct */
        jnz find_list            /* because for loop doesn't check the head */
        jmp find_found

find_list:
        pushl %edx
        call search             /* find the previous pointer of the structure with desired location */
        addl $4, %esp          

        cmpl $0, %eax
        jz invalid_value

        movl NEXT(%eax), %eax   /* update eax with its next (the structure with the location) */


find_found:
        pushl $STRUCT_SIZE
        pushl %eax
        pushl %esi
        call mp1_copy_to_user  /*unsigned long mp1 copy to user (void *to, const void *from, unsigned long n); (returns number of unavailable bytes)*/
        add $12, %esp

        cmpl $0, %eax          /* if mp1_copy_to_user malfunctioned, free the argument struct */
        jnz invalid_value      /* null = invalid */

        pop %edi
        pop %esi
        pop %ebx

        movl $0, %eax
        leave
        ret



/*
args: first half = the location of the ﬁrst blinking character
      second half = the location of the second blinking character that will be synchronized with the ﬁrst
returns: -1 if fails (can't find corresponding structures or locations are invalid)
          0 if successfully functioned
side effect: clobbers eax
functionality: find the structure with first half as its location and copy the timing info
               to the structure with second half as its location, and then it pokes     
registers: dx = second half / cx = first half  / ebx = head
           esi = the pointer that points the structure with second half as its location
           edi = the pointer that points the structure with first half as its location
*/
mp1_ioctl_sync:         /*int mp1 ioctl sync(unsigned long arg)*/
        pushl %ebp      /* push old frame pointer */
        movl %esp, %ebp /* point to new frame */
        pushl %ebx      /* ebx = head */
        pushl %esi      /* esi = the pointer that points the structure with second half as its location */
        pushl %edi      /* edi = the pointer that points the structure with first half as its location */

        movl mp1_list_head, %ebx /* ebx = head */
        cmpl $0, %ebx
        jz invalid_value  /* null = invalid */

        xorl %edx, %edx   /* dx = second half (location) */
        movw 8(%ebp), %dx
        cmpw $2000, %dx   /* check if location is greater than or equal to 2000 (invalid memeory address) */
        jnb invalid_value  

        xorl %ecx, %ecx   /* cx = first half (location) */
        movw 10(%ebp), %cx
        cmpw $2000, %cx   /* check if location is greater than or equal to 2000 (invalid memeory address) */
        jnb invalid_value

        cmpw %dx, LOCATION(%ebx) /* check if the head is the desired struct for the first half */
        jnz sync_first_pointer   /* because for loop doesn't check the head */
        movl %ebx, %esi 
        jmp sync_second_half

sync_first_pointer:
        pushl %edx
        call search             /* find the previous pointer of the structure with desired location */
        addl $4, %esp

        cmpl $0, %eax
        jz invalid_value        /* null = invalid */

        movl NEXT(%eax), %eax   /* update eax with its next */
        movl %eax, %esi         /* esi = the pointer that points the structure with first half as its location*/

sync_second_half:
        cmpw %cx, LOCATION(%ebx) /* check if the head is the desired struct for the second half */
        jnz sync_second_pointer    /* because for loop doesn't check the head */
        movl %ebx, %edi         
        jmp sync_found_both

sync_second_pointer:
        pushl %ecx
        call search           /* find the previous pointer of the structure with desired location */
        addl $4, %esp        

        cmpl $0, %eax
        jz invalid_value      /* null = invalid */

        movl NEXT(%eax), %eax   /* update eax with its next (the structure with the location) */
        movl %eax, %edi         /* edi = the pointer that points the structure with second half as its location*/

sync_found_both: 
        /* esi = pointer of the first struct / edi = pointer of the second struct
        /* copy on_length, off_length, countdown, sattus from edi to esi */
        /* eax is temp */ 
        movw ON_LENGTH(%edi), %ax
        movw %ax, ON_LENGTH(%esi)
        movw OFF_LENGTH(%edi), %ax
        movw %ax, OFF_LENGTH(%esi)
        movw COUNTDOWN(%edi), %ax
        movw %ax, COUNTDOWN(%esi)
        movw STATUS(%edi), %ax
        movw %ax, STATUS(%esi)

        cmpw $0, STATUS(%esi)
        jz sync_STATUS_off

        movb ON_CHAR(%esi), %cl /* if status != 0, push ON_CHAR to the mp1_poke*/
        jmp sync_poke

sync_STATUS_off:
        movb OFF_CHAR(%esi), %cl /* if status == 0, push OFF_CHAR to the mp1_poke*/

sync_poke:
        xorl %eax, %eax
        movw LOCATION(%esi), %ax
        shlw $1, %ax
        call mp1_poke

        pop %edi
        pop %esi
        pop %ebx
        movl $0, %eax
        leave
        ret


/*
args: location in 4bytes(only the last 2 bytes will be used),
returns: the previous pointer of the link with the argument(location) as its location
         i.e. -> [return this pointer] -> [the struct with argument as its location]
         if it can't find the desired struct or the location is invalid, it returns 0
side effect: clobbers eax
functionality:  iterate through the linked list, starting from mp1_list_head->next.
                in each struct, compare its location with the argument.
                if the struct is the desired struct, return the pointer of one before it.
                if it can't find the desired struct thus reaches its tale, return 0.
                it can't catch the case when the head is the desired struct.
                                  
registers: ebx = argument(location) / esi = current struct's pointer 
           edi = previous struct's pointer
*/
search:
        pushl %ebp      /* push old frame pointer */
        movl %esp, %ebp /* point to new frame */
        pushl %ebx
        pushl %esi
        pushl %edi

        xorl %ebx, %ebx
        movw 8(%ebp), %bx /* ebx = location */
        movl mp1_list_head, %esi
        movl NEXT(%esi), %esi
        movl mp1_list_head, %edi


/* in this iteration, check the current node's location is same as the argument.
 If so, return the previous pointer. Else check next */
check_this_struct:
        cmpl $0, %esi
        jz struct_not_found

        cmpw LOCATION(%esi), %bx
        jnz check_next_struct

        movl %edi, %eax 
        popl %edi
        popl %esi
        popl %ebx
        leave
        ret

/* update the previous pointer with the current pointer and move the current pointer to its next */
check_next_struct: 
        movl %esi, %edi
        movl NEXT(%esi), %esi
        jmp check_this_struct

/* return 0 */
struct_not_found: 
        movl $0, %eax
        popl %edi
        popl %esi
        popl %ebx
        leave
        ret

/* the struct pointed by esi will be freed and return -1 */
free_esi:
        pushl %eax
        pushl %ecx
        pushl %edx 
        pushl %esi         /*void mp1 free(void* ptr);*/
        call mp1_free
        add $4, %esp
        popl %edx
        popl %ecx
        popl %eax

/* return -1 */
invalid_value:
        pop %edi
        pop %esi
        pop %ebx
        movl $-1, %eax
        leave
        ret

jump_table:
.long mp1_ioctl_add, mp1_ioctl_remove, mp1_ioctl_find, mp1_ioctl_sync

