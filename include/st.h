#ifndef ST_H
#define ST_H

#include "token.h"
#include "type.h"

enum {
	ST_SCOPE,
	ST_LITERAL,
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

#endif // ST_H

