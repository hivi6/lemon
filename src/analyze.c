#include "analyze.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

static st_t *global_scope = NULL;

void analyzer_match(ast_t *ast, int type, const char *error_message);

void analyze_prog(st_t *scope, ast_t *ast);
void analyze_stmt(st_t *scope, ast_t *ast);
void analyze_block_stmt(st_t *scope, ast_t *ast);
void analyze_var_stmt(st_t *scope, ast_t *ast);
void analyze_expr_stmt(st_t *scope, ast_t *ast);
void analyze_expr(st_t *scope, ast_t *ast);
void analyze_binary(st_t *scope, ast_t *ast);
void analyze_literal(st_t *scope, ast_t *ast);

// ========================================
// analyzer.h - definition
// ========================================

void analyze(ast_t *ast) {
	global_scope = st_create_scope(NULL); // Creating the global scope
	analyze_prog(global_scope, ast);
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

void analyze_prog(st_t *scope, ast_t *ast) {
	analyzer_match(ast, AST_PROG, "Expected an AST_PROG ast");

	ast->scope = scope;

	for (ast_t *cur = ast->prog.asts; cur; cur = cur->next) {
		analyze_stmt(scope, cur);
	}
}

void analyze_stmt(st_t *scope, ast_t *ast) {
	ast->scope = scope;
	switch (ast->type) {
	case AST_EXPR_STMT:
		analyze_expr_stmt(scope, ast);
		break;
	case AST_BLOCK_STMT:
		analyze_block_stmt(scope, ast);
		break;
	case AST_VAR_STMT:
		analyze_var_stmt(scope, ast);
		break;
	default:
		fprintf(stderr, "what is this STMT type?\n");
		exit(1);
	}
}

void analyze_block_stmt(st_t *scope, ast_t *ast) {
	analyzer_match(ast, AST_BLOCK_STMT, "Expected an AST_BLOCK_STMT ast");

	st_t *block_scope = st_create_scope(scope);
	ast->scope = block_scope;

	for (ast_t *x = ast->block_stmt.stmts; x; x = x->next) {
		analyze_stmt(block_scope, x);
	}
}

void analyze_var_stmt(st_t *scope, ast_t *ast) {
	analyzer_match(ast, AST_VAR_STMT, "Expected an AST_VAR_STMT ast");

	token_t id = ast->var_stmt.identifier;
	if (st_check_var(scope, id)) {
		error_print(id.filepath, id.src, id.start, id.end, 
			"Variable already defined in scope");
		exit(1);
	}

	type_t *data_type = type_int();
	if (ast->var_stmt.expr) {
		analyze_expr(scope, ast->var_stmt.expr);
		data_type = ast->var_stmt.expr->data_type;
	}

	st_create_var(scope, id, data_type);
}

void analyze_expr_stmt(st_t *scope, ast_t *ast) {
	analyzer_match(ast, AST_EXPR_STMT, "Expected an AST_EXPR_STMT ast");

	analyze_expr(scope, ast->expr_stmt.expr);
}

void analyze_expr(st_t *scope, ast_t *ast) {
	ast->scope = scope;
	switch (ast->type) {
	case AST_BINARY:
		analyze_binary(scope, ast);
		break;
	case AST_LITERAL:
		analyze_literal(scope, ast);
		break;
	default:
		fprintf(stderr, "what is this EXPR type?\n");
		exit(1);
	}
}

void analyze_binary(st_t *scope, ast_t *ast) {
	analyzer_match(ast, AST_BINARY, "Expected an AST_BINARY ast");

	ast_t *left = ast->binary.left, *right = ast->binary.right;
	analyze_expr(scope, left);
	analyze_expr(scope, right);

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

void analyze_literal(st_t *scope, ast_t *ast) {
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
	if (st_check_literal(global_scope, ast->literal.token, 
		ast->data_type)) {
		return;
	}

	st_create_literal(global_scope, ast->literal.token, ast->data_type);
}

