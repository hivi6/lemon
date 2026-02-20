#ifndef AST_H
#define AST_H

#include "pos.h"
#include "token.h"
#include "type.h"

#define AST_PRINT_DEPTH 1024

enum {
	AST_LITERAL,
	AST_BINARY,
	AST_EXPR_STMT,
	AST_BLOCK_STMT,
};

struct ast_t {
	int type;
	const char *filepath;
	const char *src;
	pos_t start;
	pos_t end;

	struct ast_t *next;

	struct {
		token_t token;

		// Keep track of the type of token
		type_t *type;
	} literal;

	struct {
		struct ast_t *left;
		token_t op;
		struct ast_t *right;

		// Keep track of the type of the result of binary
		type_t *type;
	} binary;

	struct {
		struct ast_t *expr;
		token_t semicolon;
	} expr_stmt;

	struct {
		struct ast_t *stmts;
	} block_stmt;
};

typedef struct ast_t ast_t;

/**
 * Generate the ast based on the list of tokens
 *
 * Params:
 * 	tokens  List of tokens
 *
 * Returns:
 * 	Generated ast (Users responsibility to free memory)
 */
ast_t *generate_ast(token_t *tokens);

/**
 * Free ast memory
 *
 * Params:
 * 	ast  Ast that needs freeing
 */
void free_ast(ast_t *ast);

/**
 * Print the ast
 *
 * Params:
 * 	ast  Ast that needs printing
 */
void print_ast(ast_t *ast);

#endif // AST_H

