#include "analyze.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

void analyzer_match(ast_t *ast, int type, const char *error_message);

void analyze_prog(ast_t *ast);
void analyze_stmt(ast_t *ast);
void analyze_block_stmt(ast_t *ast);
void analyze_expr_stmt(ast_t *ast);
void analyze_expr(ast_t *ast);
void analyze_binary(ast_t *ast);
void analyze_literal(ast_t *ast);

// ========================================
// analyzer.h - definition
// ========================================

void analyze(ast_t *ast) {
	analyze_prog(ast);
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

void analyze_prog(ast_t *ast) {
	for (ast_t *cur = ast; cur; cur = cur->next) {
		analyze_stmt(ast);
	}
}

void analyze_stmt(ast_t *ast) {
	switch (ast->type) {
	case AST_EXPR_STMT:
		analyze_expr_stmt(ast);
		break;
	case AST_BLOCK_STMT:
		analyze_block_stmt(ast);
		break;
	default:
		fprintf(stderr, "what is this STMT type?\n");
		exit(1);
	}
}

void analyze_block_stmt(ast_t *ast) {
	analyzer_match(ast, AST_BLOCK_STMT, "Expected an AST_BLOCK_STMT ast");

	for (ast_t *x = ast->block_stmt.stmts; x; x = x->next) {
		analyze_stmt(x);
	}
}

void analyze_expr_stmt(ast_t *ast) {
	analyzer_match(ast, AST_EXPR_STMT, "Expected an AST_EXPR_STMT ast");

	analyze_expr(ast->expr_stmt.expr);
}

void analyze_expr(ast_t *ast) {
	switch (ast->type) {
	case AST_BINARY:
		analyze_binary(ast);
		break;
	case AST_LITERAL:
		analyze_literal(ast);
		break;
	default:
		fprintf(stderr, "what is this EXPR type?\n");
		exit(1);
	}
}

void analyze_binary(ast_t *ast) {
	analyzer_match(ast, AST_BINARY, "Expected an AST_BINARY ast");

	ast_t *left = ast->binary.left, *right = ast->binary.right;
	analyze_expr(left);
	analyze_expr(right);

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

void analyze_literal(ast_t *ast) {
	switch (ast->literal.token.type) {
	case TT_INT_LITERAL:
		ast->data_type = type_int();
		break;
	default:
		fprintf(stderr, "What is this literal type?\n");
		exit(1);
	}
}

