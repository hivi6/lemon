#include "st.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================================
// helper declaration
// ========================================

st_t *st_malloc(int type);
int st_scope_append(st_t *scope, st_t *sym, int size);
int is_token_equal(token_t left, token_t right);

// ========================================
// st.h - definition
// ========================================

st_t *st_create_scope(int scope_type, st_t *parent) {
	st_t *res = st_malloc(scope_type);
	res->scope.parent = parent;
	res->scope.size = 0;
	return res;
}

st_t *st_check_literal(st_t *scope, token_t token, type_t *data_type) {
	if (scope == NULL) return NULL;

	for (st_t *cur = scope->next; cur; cur = cur->next) {
		if (cur->type == ST_LITERAL && 
			cur->literal.data_type == data_type &&
			is_token_equal(cur->literal.token, token)) {
			return cur;
		}
	}

	return NULL;
}

st_t *st_create_literal(st_t *scope, token_t token, type_t *data_type) {
	st_t *sym = st_malloc(ST_LITERAL);
	sym->literal.token = token;
	sym->literal.data_type = data_type;
	sym->literal.offset = st_scope_append(scope, sym, data_type->size);
	return sym;
}

st_t *st_check_var(st_t *scope, token_t identifier) {
	if (scope == NULL) return NULL;

	for (st_t *cur = scope->next; cur; cur = cur->next) {
		if (cur->type == ST_VAR && 
			is_token_equal(cur->var.token, identifier)) {
			return cur;
		}
	}

	return NULL;
}

st_t *st_create_var(st_t *scope, token_t identifier, type_t *data_type) {
	st_t *sym = st_malloc(ST_VAR);
	sym->var.token = identifier;
	sym->var.data_type = data_type;
	sym->var.offset = st_scope_append(scope, sym, data_type->size);
	return sym;
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

int st_scope_append(st_t *scope, st_t *sym, int size) {
	st_t *cur = NULL;
	for (cur = scope; cur->next; cur = cur->next) {}
	cur->next = sym;

	int offset = scope->scope.size;
	scope->scope.size += size;
	return offset;
}

int is_token_equal(token_t left, token_t right) {
	char *cur_lex = token_lexical(left);
	char *token_lex = token_lexical(right);
	int same = (strcmp(cur_lex, token_lex) == 0);
	free(cur_lex);
	free(token_lex);
	return same;
}

