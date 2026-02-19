#include "token.h"
#include "pos.h"
#include "error.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================================
// helper declaration
// ========================================

static struct {
	const char *filepath;
	const char *src;
	int src_len;
	pos_t prev;
	pos_t cur;
	token_t *head;
	token_t *tail;
} lexer;

void lexer_init(const char *filepath, const char *src);
void lexer_error(pos_t start, pos_t end, const char *message);
int lexer_eof();
int lexer_read_token();
char lexer_current_char();
char lexer_next_char();
int lexer_append_token(int type);

// ========================================
// token.h - definition
// ========================================

token_t *generate_tokens(const char *filepath, const char *src) {
	lexer_init(filepath, src);

	while (!lexer_eof()) {
		lexer_read_token();
	}

	lexer.prev = lexer.cur;
	lexer_append_token(TT_EOF);

	return lexer.head;
}

void free_tokens(token_t *tokens) {
	while (tokens) {
		token_t *prev = tokens;
		tokens = tokens->next;
		free(prev);
	}
}

const char *token_type(token_t token) {
	if (token.type == TT_SEMICOLON) return "TT_SEMICOLON";
	if (token.type == TT_INT_LITERAL) return "TT_INT_LITERAL";
	if (token.type == TT_EOF) return "TT_EOF";
	return "UNKNOWN";
}

char *token_lexical(token_t token) {
	int len = token.end.index - token.start.index;
	const char *src = token.src + token.start.index;

	char *res = malloc((len + 1) * sizeof(char));
	if (res == NULL) {
		perror("Error in token_lexical with malloc");
		exit(1);
	}

	for (int i = 0; i < len; i++) {
		res[i] = src[i];
	}
	res[len] = '\0';

	return res;
}

// ========================================
// helper definition
// ========================================

void lexer_init(const char *filepath, const char *src) {
	lexer.filepath = filepath;
	lexer.src = src;
	lexer.cur = POS_INIT;
	lexer.prev = POS_INIT;
	lexer.head = lexer.tail = NULL;

	lexer.src_len = strlen(src);
}

void lexer_error(pos_t start, pos_t end, const char *message) {
	error_print(lexer.filepath, lexer.src, start, end, message);
	exit(1);
}

int lexer_eof() {
	return lexer.cur.index >= lexer.src_len;
}

int lexer_read_token() {
	while (!lexer_eof() && isspace(lexer_current_char())) {
		lexer_next_char();
	}

	if (lexer_eof()) return TT_EOF;

	lexer.prev = lexer.cur;
	char ch = lexer_next_char();

	if (ch == ';') {
		return lexer_append_token(TT_SEMICOLON);
	}
	else if (isdigit(ch)) {
		while (!lexer_eof() && isdigit(lexer_current_char())) {
			lexer_next_char();
		}
		return lexer_append_token(TT_INT_LITERAL);
	}

	lexer_error(lexer.prev, lexer.cur, "Unexpected character");
	return TT_EOF;
}

int lexer_append_token(int type) {
	token_t *token = malloc(sizeof(token_t));
	token->type = type;
	token->filepath = lexer.filepath;
	token->src = lexer.src;
	token->start = lexer.prev;
	token->end = lexer.cur;
	token->next = NULL;

	if (lexer.head == NULL) {
		lexer.head = lexer.tail = token;
	}
	else {
		lexer.tail->next = token;
		lexer.tail = token;
	}

	return type;
}

char lexer_current_char() {
	if (lexer_eof()) return 0;
	return lexer.src[lexer.cur.index];
}

char lexer_next_char() {
	if (lexer_eof()) return 0;

	char ch = lexer_current_char();

	if (ch == '\n') {
		lexer.cur.line++;
		lexer.cur.column = 0;
	}

	lexer.cur.index++;
	lexer.cur.column++;

	return ch;
}

