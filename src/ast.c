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
	token_t prev;
} parser;

void parser_init(token_t *tokens);
int parser_match(int type);
token_t parser_current();
token_t parser_next();
token_t parser_prev();
int parser_eof();

ast_t *parse_prog();
ast_t *parse_stmt();
ast_t *parse_block_stmt();
ast_t *parse_var_stmt();
ast_t *parse_expr_stmt();
ast_t *parse_expr();
ast_t *parse_expr_add();
ast_t *parse_expr_primary();

ast_t *ast_malloc(int type, const char *filepath, const char *src, pos_t start,
	pos_t end);
ast_t *ast_literal(token_t token);
ast_t *ast_identifier(token_t token);
ast_t *ast_binary(ast_t *left, token_t op, ast_t *right);
ast_t *ast_expr_stmt(ast_t *expr, token_t semicolon);
ast_t *ast_var_stmt(token_t var_keyword, token_t identifier, ast_t *expr, 
	token_t semicolon);
ast_t *ast_block_stmt(token_t lparen, ast_t *stmts, token_t rparen);
ast_t *ast_prog(ast_t *asts);

void print_ast_helper(ast_t *ast, int *last, int depth);
void print_token(token_t token);
void print_ast_scope_info(ast_t *ast);

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

	if (ast->next) free_ast(ast->next);

	switch (ast->type) {
	case AST_LITERAL:
	case AST_IDENTIFIER:
		break;
	case AST_BINARY:
		free_ast(ast->binary.left);
		free_ast(ast->binary.right);
		break;
	case AST_EXPR_STMT:
		free_ast(ast->expr_stmt.expr);
		break;
	case AST_BLOCK_STMT:
		free_ast(ast->block_stmt.stmts);
		break;
	case AST_PROG:
		free_ast(ast->prog.asts);
		break;
	}
	free(ast);
}

void print_ast(ast_t *ast) {
	printf("AST\n");
	int last[AST_PRINT_DEPTH] = {};
	last[0] = 0;
	print_ast_helper(ast, last, 0);
}

void print_ast_scope(ast_t *ast) {
	if (ast->type == AST_PROG) { // Print all the type info
		printf("========== TYPE INFO ==========\n");
		print_all_types();
		printf("===============================\n\n");
	}

	if (ast->type == AST_PROG) {
		print_ast_scope_info(ast);

		ast_t *start = NULL;
		if (ast->type == AST_PROG) start = ast->prog.asts;
		else start = ast->block_stmt.stmts;

		for (ast_t *cur = start; cur; cur = cur->next) {
			print_ast_scope(cur);
		}
	}
}

// ========================================
// helper definition
// ========================================

void parser_init(token_t *tokens) {
	parser.tokens = tokens;
	parser.cur = tokens;
	parser.prev.type = TT_EOF;
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
	parser.prev = token;
	parser.cur = parser.cur->next;
	return token;
}

token_t parser_prev() {
	return parser.prev;
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

	if (head == NULL) {
		fprintf(stderr, "empty program\n");
		exit(1);
	}

	return ast_prog(head);
}

ast_t *parse_stmt() {
	if (parser_match(TT_LBRACE)) {
		return parse_block_stmt();
	}
	else if (parser_match(TT_VAR_KEYWORD)) {
		return parse_var_stmt();
	}

	return parse_expr_stmt();
}

ast_t *parse_block_stmt() {
	token_t lparen = parser_prev();
	ast_t *head = NULL, *tail = NULL;
	while (!parser_match(TT_RBRACE)) {
		token_t cur = parser_current();

		if (parser_eof()) {
			error_print(cur.filepath, cur.src, cur.start, cur.end,
				"Expected '}' but reached eof");
			exit(1);
		}

		ast_t *stmt = parse_stmt();

		if (head == NULL) head = tail = stmt;
		else {
			tail->next = stmt;
			tail = stmt;
		}
	}
	return ast_block_stmt(lparen, head, parser_prev());
}

ast_t *parse_var_stmt() {
	token_t var_keyword = parser_prev();

	token_t identifier = parser_current();
	if (!parser_match(TT_IDENTIFIER)) {
		token_t cur = parser_current();
		error_print(cur.filepath, cur.src, cur.start, cur.end,
			"Expected identifier after var keyword");
		exit(1);
	}

	ast_t *expr = NULL;
	if (parser_match(TT_EQUAL)) {
		expr = parse_expr();
	}

	token_t semicolon = parser_current();
	if (!parser_match(TT_SEMICOLON)) {
		token_t cur = parser_current();
		error_print(cur.filepath, cur.src, cur.start, cur.end,
			"Expected ';' at the end of var stmt");
		exit(1);
	}

	return ast_var_stmt(var_keyword, identifier, expr, semicolon);
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
	else if (parser_match(TT_IDENTIFIER)) {
		return ast_identifier(token);
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
	res->data_type = NULL;
	res->name_scope = NULL;
	res->memory_scope = NULL;
	res->offset = -1;
	res->next = NULL;
	return res;
}

ast_t *ast_literal(token_t token) {
	ast_t *res = ast_malloc(AST_LITERAL, token.filepath, token.src,
		token.start, token.end);
	res->literal.token = token;
	return res;
}

ast_t *ast_identifier(token_t token) {
	ast_t *res = ast_malloc(AST_IDENTIFIER, token.filepath, token.src,
		token.start, token.end);
	res->identifier.token = token;
	return res;
}

ast_t *ast_binary(ast_t *left, token_t op, ast_t *right) {
	ast_t *res = ast_malloc(AST_BINARY, op.filepath, op.src, left->start,
		right->end);
	res->binary.left = left;
	res->binary.op = op;
	res->binary.right = right;
	return res;
}

ast_t *ast_expr_stmt(ast_t *expr, token_t semicolon) {
	ast_t *res = ast_malloc(AST_EXPR_STMT, expr->filepath, expr->src,
		expr->start, semicolon.end);
	res->expr_stmt.expr = expr;
	res->expr_stmt.semicolon = semicolon;
	return res;
}

ast_t *ast_var_stmt(token_t var_keyword, token_t identifier, ast_t *expr, 
	token_t semicolon) {
	ast_t *res = ast_malloc(AST_VAR_STMT, var_keyword.filepath, 
		var_keyword.src, var_keyword.start, semicolon.end);
	res->var_stmt.var_keyword = var_keyword;
	res->var_stmt.identifier = identifier;
	res->var_stmt.expr = expr;
	res->var_stmt.semicolon = semicolon;
	return res;
}

ast_t *ast_block_stmt(token_t lparen, ast_t *stmts, token_t rparen) {
	ast_t *res = ast_malloc(AST_BLOCK_STMT, lparen.filepath, lparen.src,
		lparen.start, rparen.end);
	res->block_stmt.stmts = stmts;
	return res;
}

ast_t *ast_prog(ast_t *asts) {
	pos_t start = (asts ? asts->start : POS_INIT);
	pos_t end = start;
	for (ast_t *cur = asts; cur; cur = cur->next) {
		end = cur->end;
	}

	ast_t *res = ast_malloc(AST_PROG, asts->filepath, asts->src, 
		start, end);
	res->prog.asts = asts;
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
	case AST_IDENTIFIER: {
		token_t token;
		const char *ast_type = NULL;

		if (ast->type == AST_LITERAL) {
			token = ast->literal.token;
			ast_type = "AST_LITERAL";
		}
		else {
			token = ast->identifier.token;
			ast_type = "AST_IDENTIFIER";
		}

		printf("+-- %s(", ast_type);
		print_token(token);
		printf(")\n");

		last[depth+1] = 0;
		break;
	}

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
	
	case AST_BLOCK_STMT: {
		printf("+-- AST_BLOCK_STMT\n");

		for (ast_t *x = ast->block_stmt.stmts; x; x = x->next) {
			last[depth+1] = (x->next ? 1 : 0);
			print_ast_helper(x, last, depth+1);
		}
		break;
	}

	case AST_VAR_STMT: {
		printf("+-- AST_VAR_STMT(");
		print_token(ast->var_stmt.identifier);
		printf(")\n");

		last[depth+1] = 0;
		if (ast->var_stmt.expr) {
			print_ast_helper(ast->var_stmt.expr, last, depth+1);
		}
		break;
	}
	
	case AST_PROG: {
		printf("+-- AST_PROG\n");

		for (ast_t *x = ast->prog.asts; x; x = x->next) {
			last[depth+1] = (x->next ? 1 : 0);
			print_ast_helper(x, last, depth+1);
		}
		break;
	}
	}
}

void print_token(token_t token) {
	char *lexical = token_lexical(token);
	printf("%s | %s", token_type(token), lexical);
	free(lexical);
}

void print_ast_scope_info(ast_t *ast) {
	printf("========== MEMORY_BLOCK: %p | MEMORY_SIZE: %d - NAME_BLOCK: %p | NAME_SIZE: %d ==========\n", 
		ast->memory_scope, ast->memory_scope->scope.size, ast->name_scope, ast->name_scope->scope.size);
	for (int i = ast->start.index; i < ast->end.index; i++) {
		printf("%c", ast->src[i]);
	}
	printf("\n");

	printf("xxxxxxxxxx SYMBOLS xxxxxxxxxx\n");

	for (st_t *cur = ast->memory_scope; cur; cur = cur->next) {
		switch (cur->type) {
		case ST_MEMORY_SCOPE:
		case ST_NAME_SCOPE:
			continue;
		case ST_LITERAL: 
		case ST_VAR: {
			token_t token;
			type_t *data_type;
			int offset;
			if (cur->type == ST_LITERAL) {
				token = cur->literal.token;
				data_type = cur->literal.data_type;
				offset = cur->literal.offset;
			}
			else {
				token = cur->var.token;
				data_type = cur->var.data_type;
				offset = cur->var.offset;
			}

			const char *type_str = "ST_VAR";
			if (cur->type == ST_LITERAL) type_str = "ST_LITERAL";

			char *lexical = token_lexical(token);
			int sz = 0;
			sz = snprintf(NULL, 0, "type: %-10s | id: %p(offset: %d) | type: %p(size: %d) | lexical: %s", 
				type_str, cur, offset, data_type, data_type->size, lexical);
			char *str = malloc((sz + 1) * sizeof(char));
			sprintf(str, "type: %-10s | id: %p(offset: %d) | type: %p(size: %d) | lexical: %s", 
				type_str, cur, offset, data_type, data_type->size, lexical);

			error_print(token.filepath, token.src, token.start,
				token.end, str);


			free(str);
			free(lexical);
			break;
		}
		}
		printf("\n");
	}
	printf("\n");
}

