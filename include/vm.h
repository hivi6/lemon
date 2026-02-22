#ifndef VM_H
#define VM_H

#include "ir.h"

/**
 * Run the vm with given ir list
 *
 * Params:
 * 	ir  head of ir list
 */
void run_vm(ir_t *ir);

/**
 * Print the current vm state
 */
void print_vm_state(ir_t *ir);

#endif // VM_H

