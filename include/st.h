#ifndef ST_H
#define ST_H

#include "token.h"
#include "type.h"

enum {
	ST_SCOPE,
	ST_LITERAL,
	ST_VAR,
};

struct st_t {
	int type;

	struct st_t *next;

	struct {
		struct st_t *parent;
	} scope;

	struct {
		token_t token;
		type_t *data_type;
	} literal;

	struct {
		token_t token;
		type_t *data_type;
	} var;
};

typedef struct st_t st_t;

/**
 * Create a new scope with parent scope
 *
 * Params:
 * 	parent  Parent scope
 *
 * Returns:
 * 	Memory to new scope
 */
st_t *st_create_scope(st_t *parent);

/**
 * Check if a literal exists in scope (or parent scope)
 *
 * Params:
 * 	scope      scope that needs scanning
 * 	token      token literal
 * 	data_type  type of literal
 *
 * Returns:
 * 	pointer to the literal (null if no literal)
 */
st_t *st_check_literal(st_t *scope, token_t token, type_t *data_type);

/**
 * Create a literal in scope
 *
 * Params:
 * 	scope      scope that needs scanning
 * 	token      token literal
 * 	data_type  type of literal
 */
void st_create_literal(st_t *scope, token_t token, type_t *data_type);

/**
 * Check if a variable exists (only in that scope)
 *
 * Params:
 * 	scope       scope that needs scanning
 * 	identifier  token that identifies the variable
 *
 * Returns:
 * 	pointer to the variable (null if no variable)
 */
st_t *st_check_var(st_t *scope, token_t identifier);

/**
 * Create a variable in scope of the given data type
 *
 * Params:
 * 	scope       scope that needs scanning
 * 	identifier  token that identifies the variable
 *	data_type   data type of the variable
 */
void st_create_var(st_t *scope, token_t identifier, type_t *data_type);

#endif // ST_H

