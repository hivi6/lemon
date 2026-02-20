#ifndef TOKEN_H
#define TOKEN_H

#include "pos.h"

enum {
	TT_EOF = 0,

	TT_SEMICOLON,
	TT_PLUS,
	TT_MINUS,
	TT_LBRACE,
	TT_RBRACE,

	TT_INT_LITERAL,
};

struct token_t {
	int type;
	const char *filepath;
	const char *src;
	pos_t start;
	pos_t end;

	struct token_t *next;
};

typedef struct token_t token_t;

/**
 * Generate the token based on the provided filepath and source code
 *
 * Params:
 * 	filepath  name of the file whose tokens are generated
 * 	src       source code based on which tokens are generated
 *
 * Returns:
 * 	Head of the token list (User responsible for free memory)
 */
token_t *generate_tokens(const char *filepath, const char *src);

/**
 * Free all the tokens
 *
 * Params:
 * 	token  head of the token list
 */
void free_tokens(token_t *token);

/**
 * Get the token type
 *
 * Params:
 * 	token  Token whose type is required
 *
 * Returns:
 * 	Name of the token
 */
const char *token_type(token_t token);

/**
 * Get the lexical of the token
 *
 * Params:
 * 	token  token whose lexical is required
 *
 * Returns:
 * 	Lexical of the token (Users responsibility to free memory)
 */
char *token_lexical(token_t token);

#endif // TOKEN_H

