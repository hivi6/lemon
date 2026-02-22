#include "analyze.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

static st_t *global_memory_scope = NULL;
static st_t *global_name_scope = NULL;

void analyzer_match(ast_t *ast, int type, const char *error_message);

void analyze_prog(st_t *memory_scope, st_t *name_scope, ast_t *ast);
void analyze_stmt(st_t *memory_scope, st_t *name_scope, ast_t *ast);
void analyze_block_stmt(st_t *memory_scope, st_t *name_scope, ast_t *ast);
void analyze_var_stmt(st_t *memory_scope, st_t *name_scope, ast_t *ast);
void analyze_expr_stmt(st_t *memory_scope, st_t *name_scope, ast_t *ast);
void analyze_expr(st_t *memory_scope, st_t *name_scope, ast_t *ast);
void analyze_binary(st_t *memory_scope, st_t *name_scope, ast_t *ast);
void analyze_literal(st_t *memory_scope, st_t *name_scope, ast_t *ast);
void analyze_identifier(st_t *memory_scope, st_t *name_scope, ast_t *ast);

// ========================================
// analyzer.h - definition
// ========================================

void analyze(ast_t *ast) {
	// Create the global scope
	global_memory_scope = st_create_scope(ST_MEMORY_SCOPE, NULL);
	global_name_scope = st_create_scope(ST_NAME_SCOPE, 
		global_memory_scope);
	analyze_prog(global_memory_scope, global_name_scope, ast);
}

// ========================================
// helper definition
// ========================================

void analyzer_match(ast_t *ast, int type, const char *error_message) {
	if (ast->type != type) {
		error_print(ast->filepath, ast->src, ast->start, ast->end,
			error_message);
		exit(1);
	}
}

void analyze_prog(st_t* memory_scope, st_t *name_scope, ast_t *ast) {
	analyzer_match(ast, AST_PROG, "Expected an AST_PROG ast");

	ast->memory_scope = memory_scope;
	ast->name_scope = name_scope;

	for (ast_t *cur = ast->prog.asts; cur; cur = cur->next) {
		analyze_stmt(memory_scope, name_scope, cur);
	}
}

void analyze_stmt(st_t* memory_scope, st_t *name_scope, ast_t *ast) {
	ast->memory_scope = memory_scope;
	ast->name_scope = name_scope;
	switch (ast->type) {
	case AST_EXPR_STMT:
		analyze_expr_stmt(memory_scope, name_scope, ast);
		break;
	case AST_BLOCK_STMT:
		analyze_block_stmt(memory_scope, name_scope, ast);
		break;
	case AST_VAR_STMT:
		analyze_var_stmt(memory_scope, name_scope, ast);
		break;
	default:
		fprintf(stderr, "what is this STMT type?\n");
		exit(1);
	}
}

void analyze_block_stmt(st_t* memory_scope, st_t *name_scope, ast_t *ast) {
	analyzer_match(ast, AST_BLOCK_STMT, "Expected an AST_BLOCK_STMT ast");

	st_t *block_scope = st_create_scope(ST_NAME_SCOPE, name_scope);
	ast->memory_scope = memory_scope;
	ast->name_scope = block_scope;

	for (ast_t *x = ast->block_stmt.stmts; x; x = x->next) {
		analyze_stmt(memory_scope, block_scope, x);
	}
}

void analyze_var_stmt(st_t* memory_scope, st_t *name_scope, ast_t *ast) {
	analyzer_match(ast, AST_VAR_STMT, "Expected an AST_VAR_STMT ast");

	token_t id = ast->var_stmt.identifier;
	if (st_check_var(name_scope, id)) {
		error_print(id.filepath, id.src, id.start, id.end, 
			"Variable already defined in scope");
		exit(1);
	}

	type_t *data_type = type_int();
	if (ast->var_stmt.expr) {
		analyze_expr(memory_scope, name_scope, ast->var_stmt.expr);
		data_type = ast->var_stmt.expr->data_type;
	}

	st_t *memory = st_create_var(memory_scope, id, data_type);
	st_t *name = st_create_var(name_scope, id, data_type);
	name->var.offset = memory->var.offset; // Set the memory offset value
}

void analyze_expr_stmt(st_t* memory_scope, st_t *name_scope, ast_t *ast) {
	analyzer_match(ast, AST_EXPR_STMT, "Expected an AST_EXPR_STMT ast");

	analyze_expr(memory_scope, name_scope, ast->expr_stmt.expr);
}

void analyze_expr(st_t* memory_scope, st_t *name_scope, ast_t *ast) {
	ast->memory_scope = memory_scope;
	ast->name_scope = name_scope;
	switch (ast->type) {
	case AST_BINARY:
		analyze_binary(memory_scope, name_scope, ast);
		break;
	case AST_LITERAL:
		analyze_literal(memory_scope, name_scope, ast);
		break;
	case AST_IDENTIFIER:
		analyze_identifier(memory_scope, name_scope, ast);
		break;
	default:
		fprintf(stderr, "what is this EXPR type?\n");
		exit(1);
	}
}

void analyze_binary(st_t* memory_scope, st_t *name_scope, ast_t *ast) {
	analyzer_match(ast, AST_BINARY, "Expected an AST_BINARY ast");

	ast_t *left = ast->binary.left, *right = ast->binary.right;
	analyze_expr(memory_scope, name_scope, left);
	analyze_expr(memory_scope, name_scope, right);

	ast_t *err_ast = NULL;
	if (!left->data_type) err_ast = left;
	if (!right->data_type) err_ast = right;

	if (err_ast) {
		error_print(err_ast->filepath, err_ast->src, err_ast->start,
			err_ast->end, "Expression should have data_type");
		exit(1);
	}

	ast->data_type = left->data_type;
}

void analyze_literal(st_t* memory_scope, st_t *name_scope, ast_t *ast) {
	switch (ast->literal.token.type) {
	case TT_INT_LITERAL:
		ast->data_type = type_int();
		break;
	default:
		fprintf(stderr, "What is this literal type?\n");
		exit(1);
	}

	// Keep all the literal in the global scope
	// Try to find if there is any literal
	if (st_check_literal(global_memory_scope, ast->literal.token, 
		ast->data_type)) {
		return;
	}

	st_create_literal(global_memory_scope, ast->literal.token, 
		ast->data_type);
}

void analyze_identifier(st_t* memory_scope, st_t *name_scope, ast_t *ast) {
	st_t *found = NULL;

	for (st_t *cur = name_scope; cur; cur = cur->scope.parent) {
		if (cur->type == ST_NAME_SCOPE) {
			found = st_check_var(cur, ast->identifier.token);
			if (found) {
				ast->data_type = found->var.data_type;
				return;
			}
		}
	}

	error_print(ast->filepath, ast->src, ast->start, ast->end,
		"Variable not defined");
	exit(1);
}

