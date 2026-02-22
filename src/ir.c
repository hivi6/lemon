#include "ir.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

static ir_t *global_head, *global_tail;
static st_t *global_memory_scope, *global_name_scope;

ir_t *ir_append(int type, int64_t arg1, int64_t arg2, int64_t arg3);
int new_register();

void ir_prog(ast_t *prog);
void ir_stmt(ast_t *stmt);
void ir_var_stmt(ast_t *stmt);
void ir_block_stmt(ast_t *stmt);
void ir_expr_stmt(ast_t *stmt);
int ir_expr(ast_t *expr);
int ir_binary_expr(ast_t *expr);
int ir_literal_expr(ast_t *expr);
int ir_identifier_expr(ast_t *expr);

// ========================================
// ir.h - definition
// ========================================

ir_t *generate_ir(ast_t *prog) {
	global_head = global_tail = NULL;
	ir_prog(prog);
	return global_head;
}

void print_ir(ir_t *ir_head) {
	printf("========== IR REPRESENTATION ==========\n");
	for (ir_t *cur = ir_head; cur; cur = cur->next) {
		const char *name = "UNKNOWN";
		int size = 0;

		switch (cur->type) {
		case IR_NOP:
			name = "IR_NOP";
			break;

		case IR_GLOBAL_ALLOC:
			name = "IR_GLOBAL_ALLOC";
			size = 3;
			break;

		case IR_GLOBAL_LOAD_CONST:
			name = "IR_GLOBAL_LOAD_CONST";
			size = 3;
			break;

		case IR_GLOBAL_LOAD:
			name = "IR_GLOBAL_LOAD";
			size = 3;
			break;

		case IR_LOAD_GLOBAL:
			name = "IR_LOAD_GLOBAL";
			size = 3;
			break;

		case IR_ADD:
			name = "IR_ADD";
			size = 3;
			break;

		case IR_SUB:
			name = "IR_SUB";
			size = 3;
			break;
		}

		printf("%s ", name);
		if (size >= 1) printf("%lld ", cur->arg1);
		if (size >= 2) printf("%lld ", cur->arg2);
		if (size >= 3) printf("%lld ", cur->arg3);
		printf("\n");
	}
}

// ========================================
// helper definition
// ========================================

ir_t *ir_append(int type, int64_t arg1, int64_t arg2, int64_t arg3) {
	ir_t *res = malloc(sizeof(ir_t));
	res->type = type;
	res->arg1 = arg1;
	res->arg2 = arg2;
	res->arg3 = arg3;
	res->next = NULL;

	if (global_head == NULL) global_head = global_tail = res;
	else {
		global_tail->next = res;
		global_tail = res;
	}

	return res;
}

int new_register() {
	static int total_register = 0;
	return ++total_register;
}

void ir_prog(ast_t *prog) {
	global_memory_scope = prog->memory_scope;
	global_name_scope = prog->name_scope;

	int global_size = prog->memory_scope->scope.size;
	ir_append(IR_GLOBAL_ALLOC, global_size, 0, 0);

	for (st_t *cur = global_memory_scope->next; cur; cur = cur->next) {
		int64_t offset = 0;
		int64_t size = 0;
		int64_t value = 0;
		if (cur->type == ST_LITERAL) {
			offset = cur->literal.offset;
			size = cur->literal.data_type->size;

			char *lexical = token_lexical(cur->literal.token);
			value = strtoll(lexical, NULL, 10);
			free(lexical);
		}
		else if (cur->type == ST_VAR) {
			offset = cur->var.offset;
			size = cur->var.data_type->size;
			value = 0;
		}

		ir_append(IR_GLOBAL_LOAD_CONST, offset, size, value);
	}

	for (ast_t *cur = prog->prog.asts; cur; cur = cur->next) {
		ir_stmt(cur);
	}
}

void ir_stmt(ast_t *stmt) {
	switch (stmt->type) {
	case AST_VAR_STMT:
		ir_var_stmt(stmt);
		break;
	case AST_BLOCK_STMT:
		ir_block_stmt(stmt);
		break;
	case AST_EXPR_STMT:
		ir_expr_stmt(stmt);
		break;
	default:
		fprintf(stderr, "What is this STMT type?\n");
		exit(1);
	}
}

void ir_var_stmt(ast_t *stmt) {
	token_t id = stmt->var_stmt.identifier;
	if (stmt->var_stmt.expr) {
		int offset = stmt->offset;
		int size = stmt->data_type->size;
		fflush(stdout);

		int reg = ir_expr(stmt->var_stmt.expr);

		ir_append(IR_GLOBAL_LOAD, offset, size, reg);
	}
}

void ir_block_stmt(ast_t *stmt) {
	for (ast_t *cur = stmt->block_stmt.stmts; cur; cur = cur->next) {
		ir_stmt(cur);
	}
}

void ir_expr_stmt(ast_t *stmt) {
	ir_expr(stmt->expr_stmt.expr);
}

int ir_expr(ast_t *expr) {
	switch (expr->type) {
	case AST_LITERAL:
		return ir_literal_expr(expr);
	case AST_IDENTIFIER:
		return ir_identifier_expr(expr);
	case AST_BINARY:
		return ir_binary_expr(expr);
	default:
		fprintf(stderr, "What is this EXPR type?\n");
		exit(1);
	}
}

int ir_literal_expr(ast_t *expr) {
	int reg = new_register();
	ir_append(IR_LOAD_GLOBAL, reg, expr->offset, expr->data_type->size);
	return reg;
}

int ir_binary_expr(ast_t *expr) {
	int left_reg = ir_expr(expr->binary.left);
	int right_reg = ir_expr(expr->binary.right);

	switch (expr->binary.op.type) {
	case TT_PLUS: {
		int res = new_register();
		ir_append(IR_ADD, res, left_reg, right_reg);
		return res;
	}
	case TT_MINUS: {
		int res = new_register();
		ir_append(IR_SUB, res, left_reg, right_reg);
		return res;
	}
	default:
		fprintf(stderr, "What is this BINARY_OP type?\n");
		exit(1);
	}
}

int ir_identifier_expr(ast_t *expr) {
	int reg = new_register();
	ir_append(IR_LOAD_GLOBAL, reg, expr->offset, expr->data_type->size);
	return reg;
}

