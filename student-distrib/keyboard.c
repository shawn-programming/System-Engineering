#include "keyboard.h"

static uint8_t shift;
static uint8_t capslock;
static uint8_t capsChecker;
static uint8_t control;
static uint8_t left_alt;
static uint8_t keybufferCount[MAX_ARG_LEN];
static uint16_t cursor[MAX_TERMINAL];
static uint8_t keyBuf1[KEYBUFFER_LENGTH];
static uint8_t keyBuf2[KEYBUFFER_LENGTH];
static uint8_t keyBuf3[KEYBUFFER_LENGTH];
static uint8_t* keyBuf[MAX_TERMINAL] = {keyBuf1, keyBuf2, keyBuf3};
static uint8_t readFlag[MAX_TERMINAL];
uint8_t current_terminal = 0;

/* keymap
   this is the keymap for the keyboard given by scan code set 1 on http://wiki.osdev.org/PS/2_Keyboard
   function:   we match the values given from the keyboard of the keypresses and unpresses to their corresponding ascii value
*/
uint8_t keymap[] = {
    0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s',
    'd','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v',
    'b','n','m',',','.','/',0,0,0,' ',0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/* keymapShift
   this is the keymap of uppercase letters for the keyboard given by scan code set 1 on http://wiki.osdev.org/PS/2_Keyboard
   function:   we match the values given from the keyboard of the keypresses and unpresses to their corresponding ascii value
*/
uint8_t keymapShift[] = {
    0,0,'!','@','3','$','%','^','&','*','(',')','_','+',0,0,
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,'A','S',
    'D','F','G','H','J','K','L',':','\"','~',0,'|','Z','X','C','V',
    'B','N','M','<','>','?',0,0,0,' ',0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/* keymapCaplock
   this is the keymap of uppercase letters for the keyboard given by scan code set 1 on http://wiki.osdev.org/PS/2_Keyboard
   function:   we match the values given from the keyboard of the keypresses and unpresses to their corresponding ascii value
*/
uint8_t keymapCapslock[] = {
    0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
    'Q','W','E','R','T','Y','U','I','O','P','[',']','\n',0,'A','S',
    'D','F','G','H','J','K','L',';','\'','`',0,'\\','Z','X','C','V',
    'B','N','M',',','.','/',0,0,0,' ',0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/* 
 * keyboard_init
 *   DESCRIPTION: this function initializes the correct port on the PIC for
 *                the keyboard, initializes all local variables to 0, and
 *                enables the cursor
 *   INPUTS:      none
 *   OUTPUTS:     none
 */
void keyboard_init(){
    //set all local variables to off
    shift = OFF;
    capslock = OFF;
    capsChecker = OFF;
    control = OFF;
    left_alt = OFF;
    keybufferCount[TERM_0] = OFF;
    keybufferCount[TERM_1] = OFF;
    keybufferCount[TERM_2] = OFF;
    readFlag[TERM_0] = UNLOCK;
    readFlag[TERM_1] = UNLOCK;
    readFlag[TERM_2] = UNLOCK;

    cursor[TERM_0] = CURSOR_START;
    cursor[TERM_1] = CURSOR_START;
    cursor[TERM_2] = CURSOR_START;
    //enable the cursor
    enable_cursor(CURSOR_START, NUM_ROWS);
    //initialize the keyboard on the PIC
    enable_irq(KEYBOARD_CONTROLLER_IRQ);
}

/*
    keyboard_handler
    function:       this function maps the keypress and then displays the corresponding
                    key or effect to the display
    input:          none
    output:         return 0
    side effect:    none
    magic numbers:  none
*/
uint32_t keyboard_handler(){
    //tell PIC that the interrupt has been handled
    send_eoi(KEYBOARD_CONTROLLER_IRQ);
    //get the key character from the keyboard
    uint8_t key_code = inb(KEYBOARD_CONTROLLER_PORT);
    //if one of the shifts is released then turn off the shift variable
    if(key_code == L_SHIFT_RELEASE || key_code == R_SHIFT_RELEASE){
        shift = OFF;
    }
    //if one of the shifts is pressed then turn the shift variable on
    else if(key_code == L_SHIFT_PRESSED || key_code == R_SHIFT_PRESSED){
        shift = ON;
    }
    //if caps lock is pressed and capslock varaible is off then turn caps lock varaible on and go to the next state
    if(key_code == CAPSLOCK_PRESSED && capsChecker == CAPSLOCK_STATE_0){
        capslock = ON;
        capsChecker = CAPSLOCK_STATE_1;
    }
    //wait to go to the next caps lock state until capslock is unpressed
    else if(key_code == CAPSLOCK_RELEASE && capsChecker == CAPSLOCK_STATE_1){
        capsChecker = CAPSLOCK_STATE_2;
    }
    //if caps lock is pressed and capslock varaible is on then turn caps lock off and go to the next state
    else if(key_code == CAPSLOCK_PRESSED && capsChecker == CAPSLOCK_STATE_2){
        capslock = OFF;
        capsChecker = CAPSLOCK_STATE_3;
    }
    //wait to go to the next caps lock state until capslock is unpressed
    else if(key_code == CAPSLOCK_RELEASE && capsChecker == CAPSLOCK_STATE_3){
        capsChecker = CAPSLOCK_STATE_0;
    }
    //if control is presesd then turn its variable on
    if(key_code == CONTROL_PRESSED){
        control = ON;
    }
    //if control is released then turn its variable off
    else if(key_code == CONTROL_RELEASE){
        control = OFF;
    }
    
    if(key_code == LEFT_ALT){
        left_alt = ON;
    }
    if(key_code == LEFT_ALT_RELEASE){
        left_alt = OFF;
    }
    //if backspace is pressed then delete the previous key and go back one on the keybuffer
    if(key_code == BACKSPACE_PRESSED){
        if(keybufferCount[current_terminal] > 0){
            delete();
            keybufferCount[current_terminal]--;
        }
        return SUCCESS;
    }
    // if tab is pressed then put for space keys on screen and in the buffer
    if(key_code == TAB_PRESSED){
        uint8_t i;
        for(i = 0; i < TAB_LENGTH; i++){
            if(keybufferCount[current_terminal]<KEYBOARD_BUFFER_SIZE){
                keyBuf[current_terminal][keybufferCount[current_terminal]] = SPACE;
                keybufferCount[current_terminal]++;
                putc(SPACE);
            }
        }
        return SUCCESS;
    }
    //if control + L is hit then clear the screen and set the cursor to the top left
    if(control == ON && key_code == L_KEY){
        clear();
        clear_set();
        if(readFlag==OFF){
            keybufferCount[current_terminal] = 0;
        }
        return SUCCESS;
    }

    //switching terminals
    if(left_alt == ON && key_code == F1_KEY){
        terminal_switch(TERM_0);
        return SUCCESS;
    }
    if(left_alt == ON && key_code == F2_KEY){     
        terminal_switch(TERM_1);
        return SUCCESS;
    }
    if(left_alt == ON && key_code == F3_KEY){       
        terminal_switch(TERM_2);
        return SUCCESS;
    }

    // if control + c is called then halt the current program
    if(control == ON && key_code == C_KEY){
        putc(NEWLINE);
        halt(CTRL_CLEAR_VAL);
    }
    else{
        uint8_t key_mapped = 0;
        //XOR shift and capslock to see if it should be uppercase
        if((shift ^ capslock) == ON){
            if(shift == ON){
                key_mapped = keymapShift[key_code];
            }
            else{
                key_mapped = keymapCapslock[key_code];
            }
        }
        else{
            key_mapped = keymap[key_code];
        }
        // if the key isnt null then print to screen
        if(key_mapped != 0){
            // if the key pressed with enter
            if(key_mapped == NEWLINE){
                // put \n in the buffer then send a done flag to terminal read and reset the buffer count to 0
                keyBuf[current_terminal][keybufferCount[current_terminal]] = key_mapped;
                // readFlag = UNLOCK;
                readFlag[current_terminal] = UNLOCK;
                keybufferCount[current_terminal] = 0;
                //show the return on screen
                putc(key_mapped);
            }
            // if the buffer isnt full
            else if(keybufferCount[current_terminal] < KEYBOARD_BUFFER_SIZE){
                //put the key in the buffer and increment the buffer
                keyBuf[current_terminal][keybufferCount[current_terminal]] = key_mapped;
                keybufferCount[current_terminal]++;
                //put the key onscreen
                putc(key_mapped);
            }
        }   
    }
    return SUCCESS;
}

/* 
 * terminal_open
 *   DESCRIPTION: Opens the terminal driver
 *   INPUTS: filename -- the file to be opened, unused
 *   OUTPUTS: returns 0 after success
 */
int32_t terminal_open(const uint8_t* filename){
    return SUCCESS;
}

/* 
 * terminal_read
 *   DESCRIPTION: Reads the contents of the keyboard buffer after an enter
 *   has been detected
 *   INPUTS: fd -- file descriptor, unused
 *           buf -- the file to read into
 *           nbytes -- the number of bytes
 *   OUTPUTS: the bytes read
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
    readFlag[curr_scheduler] = LOCK;
    uint8_t* buffer = (uint8_t*)buf;
    if(buffer == NULL || nbytes < 0) 
        return FAILURE;
    while(readFlag[curr_scheduler] == LOCK){}    
    int i; //buffer index
    int counter = 0;
    for (i = 0; i < nbytes; i++) {
        counter++;
        if(i == (nbytes - 1)){
            buffer[i] = NEWLINE;
            break;
        }

        buffer[i] = keyBuf[curr_scheduler][i];

        if (keyBuf[curr_scheduler][i] == NEWLINE)
            break;
    }
    return counter;
}

/* 
 * terminal_write
 *   DESCRIPTION: Writes the contents of the buffer
 *   INPUTS: fd -- file descriptor, unused
 *           buf -- the file to read into
 *           nbytes -- the number of bytes
 *   OUTPUTS: the bytes written
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    uint8_t* buffer = (uint8_t*)buf;
    if(buffer == NULL || nbytes < 0) 
        return FAILURE;
    int i; //the bytes written
    for (i = 0; i < nbytes; i++) {
        putc(buffer[i]);
    }
    return i;
}

/* 
 * terminal_close
 *   DESCRIPTION: Closes the terminal driver
 *   INPUTS: filename -- the file to be closed, unused
 *   OUTPUTS: returns 0 after success
 */
int32_t terminal_close(int32_t fd){
    return SUCCESS;
}

/* 
 * terminal_close
 *   DESCRIPTION: Switches from the current terminal to the new one
 *   INPUTS: new_terminal -- the terminal we want to switch to
 *   OUTPUTS: returns 0 after success
 */
int32_t terminal_switch(int32_t new_terminal){
    swap_vid_mem(new_terminal); 
    swap_kb_cursor(new_terminal);
    current_terminal = new_terminal;

    if(curr_scheduler == current_terminal){
        page_table_video[0] = (uint32_t)video_mem | vid_mem_flag;
    }
    else{
        page_table_video[0] = (uint32_t)vid_page[curr_scheduler] | vid_mem_flag;
    }

    flush_tlb();
    return 0;
}

/* 
 * swap_kb_cursor
 *   DESCRIPTION: swap the cursor between terminals
 *   INPUTS: new_terminal -- the terminal we want to switch to
 *   OUTPUTS: none
 */
void swap_kb_cursor(int32_t new_terminal){
    change_terminal_cursor(new_terminal);
}

/* 
 *  swap_vid_mem
 *   DESCRIPTION: Copys the vid_mem into the old terminal, then copies the new terminal
 *                into vid_mem
 *   INPUTS: new_terminal -- the terminal we want to switch to
 *   OUTPUTS: none
 */
void swap_vid_mem(int32_t new_terminal){
    // store the data in the video memory to the terminal memory we are switching from
    memcpy((void*)(term_1_mem + current_terminal * KB4), (void*)video_mem, KB4);
    // clear the vid_mem
    clear();
    // restore the new terminal's data into the video memory
    memcpy((void*)video_mem, (void*)(term_1_mem + new_terminal * KB4), KB4);
}

/*
 * terminal_init
 *  DESCRIPTION: Initializes the 3 terminals
 *  INPUTS: none
 *  OUTPUTS: none
 */
void terminal_init(){
    // set each of the 3 terminals to NULL
    terminal[TERM_0].pcb = NULL;
    terminal[TERM_1].pcb = NULL;
    terminal[TERM_2].pcb = NULL;
}
