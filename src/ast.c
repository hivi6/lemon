#include "ast.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

static struct {
	token_t *tokens;
	token_t *cur;
} parser;

void parser_init(token_t *tokens);
int parser_match(int type);
token_t parser_current();
token_t parser_next();
int parser_eof();

ast_t *parse_prog();
ast_t *parse_stmt();
ast_t *parse_expr_stmt();
ast_t *parse_expr();
ast_t *parse_expr_add();
ast_t *parse_expr_primary();

ast_t *ast_malloc(int type, const char *filepath, const char *src, pos_t start,
	pos_t end);
ast_t *ast_literal(token_t token);
ast_t *ast_binary(ast_t *left, token_t op, ast_t *right);
ast_t *ast_expr_stmt(ast_t *expr, token_t semicolon);

void print_ast_helper(ast_t *ast, int *last, int depth);
void print_token(token_t token);

// ========================================
// ast.h - definition
// ========================================

ast_t *generate_ast(token_t *tokens) {
	parser_init(tokens);

	ast_t *res = parse_prog();
	return res;
}

void free_ast(ast_t *ast) {
	if (ast == NULL) return;

	if (ast->next) {
		free_ast(ast->next);
	}

	switch (ast->type) {
	case AST_LITERAL:
		break;
	case AST_BINARY:
		free_ast(ast->binary.left);
		free_ast(ast->binary.right);
		break;
	case AST_EXPR_STMT:
		free_ast(ast->expr_stmt.expr);
		break;
	}
	free(ast);
}

void print_ast(ast_t *ast) {
	printf("AST\n");
	for (ast_t *cur = ast; cur; cur = cur->next) {
		int last[AST_PRINT_DEPTH] = {};
		last[0] = (cur->next ? 1 : 0);
		print_ast_helper(cur, last, 0);
	}
}

// ========================================
// helper definition
// ========================================

void parser_init(token_t *tokens) {
	parser.tokens = tokens;
	parser.cur = tokens;
}

int parser_match(int type) {
	if (parser_current().type == type) {
		parser_next();
		return 1;
	}

	return 0;
}

token_t parser_current() {
	return *parser.cur;
}

token_t parser_next() {
	token_t token = parser_current();
	parser.cur = parser.cur->next;
	return token;
}

int parser_eof() {
	return parser_current().type == TT_EOF;
}

ast_t *parse_prog() {
	ast_t *head = NULL, *tail = NULL;

	while (!parser_eof()) {
		ast_t *stmt = parse_stmt();

		if (head == NULL) {
			head = tail = stmt;
		}
		else {
			tail->next = stmt;
			tail = tail->next;
		}
	}

	return head;
}

ast_t *parse_stmt() {
	return parse_expr_stmt();
}

ast_t *parse_expr_stmt() {
	ast_t *expr = parse_expr();
	token_t token = parser_current();
	if (!parser_match(TT_SEMICOLON)) {
		error_print(expr->filepath, expr->src, expr->start, expr->end,
			"Expected ';' after expr");
		exit(1);
	}
	return ast_expr_stmt(expr, token);
}

ast_t *parse_expr() {
	return parse_expr_add();
}

ast_t *parse_expr_add() {
	ast_t *left = parse_expr_primary();
	while (parser_current().type == TT_PLUS || 
		parser_current().type == TT_MINUS) {
		token_t op = parser_next();
		left = ast_binary(left, op, parse_expr_primary());
	}
	return left;
}

ast_t *parse_expr_primary() {
	token_t token = parser_current();
	if (parser_match(TT_INT_LITERAL)) {
		return ast_literal(token);
	}

	error_print(token.filepath, token.src, token.start, token.end,
		"Expected primary");
	exit(1);
}


ast_t *ast_malloc(int type, const char *filepath, const char *src, pos_t start,
	pos_t end) {
	ast_t *res = malloc(sizeof(ast_t));
	res->type = type;
	res->filepath = filepath;
	res->src = src;
	res->start = start;
	res->end = end;
	res->next = NULL;
	return res;
}

ast_t *ast_literal(token_t token) {
	ast_t *res = ast_malloc(AST_LITERAL, token.filepath, token.src,
		token.start, token.end);
	res->literal.token = token;
	res->binary.type = NULL;
	return res;
}

ast_t *ast_binary(ast_t *left, token_t op, ast_t *right) {
	ast_t *res = ast_malloc(AST_BINARY, op.filepath, op.src, left->start,
		right->end);
	res->binary.left = left;
	res->binary.op = op;
	res->binary.right = right;
	res->binary.type = NULL;
	return res;
}

ast_t *ast_expr_stmt(ast_t *expr, token_t semicolon) {
	ast_t *res = ast_malloc(AST_EXPR_STMT, expr->filepath, expr->src,
		expr->start, semicolon.end);
	res->expr_stmt.expr = expr;
	res->expr_stmt.semicolon = semicolon;
	return res;
}

void print_ast_helper(ast_t *ast, int *last, int depth) {
	if (depth+1 >= AST_PRINT_DEPTH) {
		printf("Very long ast depth, not printing the rest");
	}

	for (int i = 0; i < depth; i++) {
		if (last[i]) printf("|   ");
		else printf("    ");
	}

	last[depth+1] = 1;
	switch (ast->type) {
	case AST_LITERAL:
		printf("+-- AST_LITERAL(");
		print_token(ast->literal.token);
		printf(")\n");

		last[depth+1] = 0;
		break;

	case AST_BINARY:
		printf("+-- AST_BINARY(");
		print_token(ast->binary.op);
		printf(")\n");

		print_ast_helper(ast->binary.left, last, depth+1);
		last[depth+1] = 0;
		print_ast_helper(ast->binary.right, last, depth+1);
		break;
	
	case AST_EXPR_STMT:
		printf("+-- AST_EXPR_STMT\n");

		last[depth+1] = 0;
		print_ast_helper(ast->expr_stmt.expr, last, depth+1);
		break;
	}
}

void print_token(token_t token) {
	char *lexical = token_lexical(token);
	printf("%s | %s", token_type(token), lexical);
	free(lexical);
}

