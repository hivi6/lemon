#include "st.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

// ========================================
// helper declaration
// ========================================

st_t *st_malloc(int type);
void st_scope_append(st_t *scope, st_t *sym);

// ========================================
// st.h - definition
// ========================================

st_t *st_create_scope(st_t *parent) {
	st_t *res = st_malloc(ST_SCOPE);
	res->scope.parent = parent;
	return res;
}

st_t *st_check_literal(st_t *scope, token_t token, type_t *data_type) {
	if (scope == NULL) return NULL;

	assert(scope->type == ST_SCOPE);

	for (st_t *cur = scope->next; cur; cur = cur->next) {
		if (cur->type == ST_LITERAL && 
			cur->literal.data_type == data_type) {
			char *cur_lex = token_lexical(cur->literal.token);
			char *token_lex = token_lexical(token);
			int same = (strcmp(cur_lex, token_lex) == 0);
			free(cur_lex);
			free(token_lex);
			if (same) return cur;
		}
	}

	return st_check_literal(scope->scope.parent, token, data_type);
}

void st_create_literal(st_t *scope, token_t token, type_t *data_type) {
	assert(scope->type == ST_SCOPE);

	st_t *sym = st_malloc(ST_LITERAL);
	sym->literal.token = token;
	sym->literal.data_type = data_type;

	st_scope_append(scope, sym);
}

// ========================================
// helper definition
// ========================================

st_t *st_malloc(int type) {
	st_t *res = malloc(sizeof(st_t));
	res->type = type;
	res->next = NULL;
	return res;
}

void st_scope_append(st_t *scope, st_t *sym) {
	assert(scope->type == ST_SCOPE);

	st_t *cur = NULL;
	for (cur = scope; cur->next; cur = cur->next) {}

	cur->next = sym;
}
