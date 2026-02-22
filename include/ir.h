#ifndef IR_H
#define IR_H

#include "ast.h"

#include <stdint.h>

enum {
	// No operation
	// No arguments
	IR_NOP,

	// Create the global memory scope
	// arg1 = size of global memory scope;
	IR_GLOBAL_ALLOC,

	// Load a given offset and size with a literal value
	// arg1 = offset; 
	// arg2 = size (max 8 bytes); 
	// arg3 = 64 bit int;
	IR_GLOBAL_LOAD_CONST,

	// Load a given offset and size with the value of register
	// arg1 = offset; 
	// arg2 = size (max 8 bytes); 
	// arg3 = register;
	IR_GLOBAL_LOAD,

	// Load a given register with the global memory offset and size
	// arg1 = register; 
	// arg2 = offset; 
	// arg3 = size (max 8 bytes);
	IR_LOAD_GLOBAL, 

	// Add the content of 2 register index and set to another register
	// arg1 = register (lhs); 
	// arg2 = register (left operand); 
	// arg3 = register (right operand);
	IR_ADD,
	
	// Subtract the content of 2 register index and set to another register
	// arg1 = register (lhs); 
	// arg2 = register (left operand); 
	// arg3 = register (right operand);
	IR_SUB,

	// Print the content of a register
	// arg1 = register
	IR_PRINT,
};

struct ir_t {
	int type;
	int64_t arg1;
	int64_t arg2;
	int64_t arg3;

	struct ir_t *next;
};

typedef struct ir_t ir_t;

/**
 * Generate ir list given the ast
 *
 * Params:
 * 	prog  program ast
 *
 * Returns:
 * 	head to the ir list
 */
ir_t *generate_ir(ast_t *prog);

/**
 * Print the ir list
 *
 * Params:
 * 	ir_head  head of the ir list
 */
void print_ir(ir_t *ir_head);

#endif // IR_H

