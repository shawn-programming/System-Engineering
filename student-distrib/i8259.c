/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"


/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = MASTER_MASK; /* IRQs 0-7  */
uint8_t slave_mask = SLAVE_MASK;  /* IRQs 8-15 */

/* 
 * i8259_init
 *   DESCRIPTION: Initialize the 8259 PIC
 *   to the ports.
 *   INPUTS: none
 *   OUTPUTS: none
 */

void i8259_init(void) {
    // Send ICW1 to first port of master and slave PICs 
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);

    // Send ICW1 to first port of master and slave PICs 
    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);

    // Send ICW1 to first port of master and slave PICs 
    outb(ICW3_MASTER, MASTER_8259_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_DATA);

    // Send ICW1 to first port of master and slave PICs 
    outb(ICW4, MASTER_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);

    // Mask all interrupts initially so they can be enabled when necessary
    outb(master_mask, MASTER_8259_DATA);
    outb(slave_mask, SLAVE_8259_DATA);

    // Enable slave PIC
    enable_irq(SLAVE_PORT);
}

/* 
 * enable_irq
 *   DESCRIPTION: Enable (unmask) the specified IRQ
 *   INPUTS: irq_num -- the IRQ to be enabled
 *   OUTPUTS: none
 */
void enable_irq(uint32_t irq_num) {
    // Check if irq_num is in bounds 0-15
    if(irq_num < 0 || irq_num >= MAX_PORTS) 
        return;
    uint8_t mask = 0;
    // Depending on whether it is master or slave PIC
    if(irq_num < NUM_PORTS) 
    {
        // Make the bit indicated by the irq_num 1
        mask = 1 << irq_num;
        // Flip mask so all bits are 1 except bit indicated by irq_num
        master_mask &= ~mask;
        // Write mask into port
        outb(master_mask, MASTER_8259_DATA);
    } 
    else 
    {
        // Make the bit indicated by the irq_num 1
        mask = 1 << (irq_num - NUM_PORTS);
        // Flip mask so all bits are 1 except bit indicated by irq_num
        slave_mask &= ~mask;
        // Write mask into port
        outb(slave_mask, SLAVE_8259_DATA);
    }
}

/* 
 * disable_irq
 *   DESCRIPTION: Disable (mask) the specified IRQ
 *   INPUTS: irq_num -- the IRQ to be enabled
 *   OUTPUTS: none
 */
void disable_irq(uint32_t irq_num) {
    // Check if irq_num is in bounds 0-15
    if(irq_num < 0 || irq_num >= MAX_PORTS) 
        return;
    uint8_t mask = 0;
    // Depending on whether it is master or slave PIC
    if(irq_num < NUM_PORTS) 
    {
        // Make the bit indicated by the irq_num 1
        mask = 1 << irq_num;
        // Set the corresponding bit of the mask
        master_mask |= mask;
        // Write mask into port
        outb(master_mask, MASTER_8259_DATA);
    } 
    else 
    {
        // Make the bit indicated by the irq_num 1
        mask = 1 << (irq_num - NUM_PORTS);
        // Set the corresponding bit of the mask
        slave_mask |= ~mask;
        // Write mask into port
        outb(slave_mask, SLAVE_8259_DATA);
    }
}

/* 
 * send_eoi
 *   DESCRIPTION: Send end-of-interrupt signal for the specified IRQ
 *   INPUTS: irq_num -- the IRQ to be enabled
 *   OUTPUTS: none
 */
void send_eoi(uint32_t irq_num) {
    // Check if irq_num is in bounds 0-15
    if(irq_num < 0 || irq_num >= MAX_PORTS) 
        return;
    // Depending on whether it is master or slave PIC
    if(irq_num < NUM_PORTS)
    {
        // Send EOI to master PIC
		outb((EOI | irq_num), MASTER_8259_PORT);
	}
	else
    {
        // Send EOI to slave PIC
		outb((EOI | (irq_num - NUM_PORTS)), SLAVE_8259_PORT);
        // Send EOI to master PIC
		outb((EOI | SLAVE_PORT), MASTER_8259_PORT);
	}
}

