#ifndef AST_H
#define AST_H

#include "pos.h"
#include "token.h"

#define AST_PRINT_DEPTH 1024

enum {
	AST_LITERAL,
	AST_EXPR_STMT,
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
	} literal;

	struct {
		struct ast_t *expr;
		token_t semicolon;
	} expr_stmt;
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

