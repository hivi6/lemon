#ifndef AST_H
#define AST_H

#include "pos.h"
#include "token.h"
#include "type.h"
#include "st.h"

#define AST_PRINT_DEPTH 1024

enum {
	AST_LITERAL,
	AST_IDENTIFIER,
	AST_BINARY,
	AST_EXPR_STMT,
	AST_BLOCK_STMT,
	AST_VAR_STMT,
	AST_PROG,
};

struct ast_t {
	int type;
	const char *filepath;
	const char *src;
	pos_t start;
	pos_t end;

	// Keep track of the datatype of any and scope
	type_t *data_type;
	st_t *memory_scope;
	st_t *name_scope;
	int offset;

	struct ast_t *next;

	struct {
		token_t token;
	} literal;

	struct {
		token_t token;
	} identifier;

	struct {
		struct ast_t *left;
		token_t op;
		struct ast_t *right;
	} binary;

	struct {
		struct ast_t *expr;
		token_t semicolon;
	} expr_stmt;

	struct {
		token_t var_keyword;
		token_t identifier;
		struct ast_t *expr;
		token_t semicolon;
	} var_stmt;

	struct {
		struct ast_t *stmts;
	} block_stmt;

	struct {
		struct ast_t *asts;
	} prog;
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

/**
 * Print the scope information
 *
 * Params:
 * 	ast  Ast that needs printing
 */
void print_ast_scope(ast_t *ast);

#endif // AST_H

