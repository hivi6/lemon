#include "vm.h"

#include <stdio.h>
#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

static char *global;
static int64_t *regs;
static int total_regs;

int64_t register_get(int64_t index);
void register_set(int64_t index, int64_t value);

// ========================================
// vm.h - definition
// ========================================

void run_vm(ir_t *ir) {
	global = NULL;
	regs = NULL;
	total_regs = 0;

	ir_t *ip = ir;

	while (ip) {
		switch (ip->type) {
		case IR_NOP:
			break;
		case IR_GLOBAL_ALLOC: {
			global = malloc(ip->arg1);
			break;
		}
		case IR_GLOBAL_LOAD_CONST:
		case IR_GLOBAL_LOAD: {
			int64_t offset = ip->arg1;
			int64_t size = ip->arg2;
			int64_t value = ip->arg3;
			if (ip->type == IR_GLOBAL_LOAD) {
				value = register_get(ip->arg3);
			}

			for (int64_t i = 0; i < size; i++) {
				int64_t msb = value >> ((size - 1 - i) * 8);
				msb &= 0b11111111;
				global[i+offset] = msb;
			}
			break;
		}
		case IR_LOAD_GLOBAL: {
			int64_t reg = ip->arg1;
			int64_t offset = ip->arg2;
			int64_t size = ip->arg3;

			int64_t value = 0;
			for (int i = 0; i < size; i++) {
				value = (value << 8) + global[offset + i];
			}

			register_set(reg, value);
			break;
		}
		case IR_ADD: {
			int64_t left = register_get(ip->arg2);
			int64_t right = register_get(ip->arg3);
			register_set(ip->arg1, left + right);
			break;
		}
		case IR_SUB: {
			int64_t left = register_get(ip->arg2);
			int64_t right = register_get(ip->arg3);
			register_set(ip->arg1, left - right);
			break;
		}
		case IR_PRINT: {
			int64_t value = register_get(ip->arg1);
			printf("%lld\n", value);
			break;
		}
		}

		ip = ip->next;
	}
}

void print_vm_state(ir_t *ir) {
	printf("========== GLOBAL STATE ==========\n");

	ir_t *ip = ir;

	while (ip) {
		switch (ip->type) {
		case IR_GLOBAL_LOAD_CONST: {
			int64_t offset = ip->arg1;
			int64_t size = ip->arg2;
			int64_t value = 0;
			for (int i = 0; i < size; i++) {
				value = (value << 8) + global[offset + i];
			}
			printf("%lld %lld: %lld\n", offset, size, value);
			break;
		}
		}

		ip = ip->next;
	}
}

// ========================================
// helper definition
// ========================================

int64_t register_get(int64_t index) {
	if (index >= total_regs) {
		total_regs = index + 1;
		regs = realloc(regs, total_regs * sizeof(int64_t));
	}
	return regs[index];
}

void register_set(int64_t index, int64_t value) {
	if (index >= total_regs) {
		total_regs = index + 1;
		regs = realloc(regs, total_regs * sizeof(int64_t));
	}
	regs[index] = value;
}

