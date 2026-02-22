#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "ast.h"
#include "util.h"
#include "analyze.h"
#include "ir.h"

// ========================================
// helper declaration
// ========================================

void usage(FILE *fd);

// ========================================
// main definition
// ========================================

int main(int argc, const char **argv) {
	int arg_index = 1;

	int flag_usage = 0;
	int tokens_flag = 0;
	int ast_flag = 0;
	int st_flag = 0;

	while (arg_index < argc) {
		if (strcmp("--help", argv[arg_index]) == 0 ||
			strcmp("-h", argv[arg_index]) == 0) {
			flag_usage = 1;
		}
		else if (strcmp("--only-tokens", argv[arg_index]) == 0) {
			tokens_flag = 1;
		}
		else if (strcmp("--only-ast", argv[arg_index]) == 0) {
			ast_flag = 1;
		}
		else if (strcmp("--only-st", argv[arg_index]) == 0) {
			st_flag = 1;
		}
		else break;

		arg_index++;
	}

	if (flag_usage) {
		usage(stdout);
		return 0;
	}

	if (arg_index >= argc) {
		fprintf(stderr, "ERROR: No source files provided\n");
		usage(stderr);
		return 1;
	}

	const char *filepath = argv[arg_index];
	char *src = read_file(filepath);

	token_t *tokens = generate_tokens(filepath, src);

	if (tokens_flag) {
		for (token_t *cur = tokens; cur; cur = cur->next) { 
			char *lexical = token_lexical(*cur);
			printf("%s | %s\n", token_type(*cur), lexical);
			free(lexical);
		}
		return 0;
	}

	ast_t *ast = generate_ast(tokens);

	if (ast_flag) {
		print_ast(ast);
		return 0;
	}

	analyze(ast);

	if (st_flag) {
		print_ast_scope(ast);
		return 0;
	}

	print_ast_scope(ast);
	ir_t *ir = generate_ir(ast);
	print_ir(ir);

	free_ast(ast);
	free_tokens(tokens);
	free(src);

	return 0;
}

// ========================================
// helper definition
// ========================================

void usage(FILE *fd) {
	fprintf(fd, "USAGE: ./lemon [flags] <filename>\n");
	fprintf(fd, "\n");
	fprintf(fd, "FLAGS:\n");
	fprintf(fd, "    --help, -h     This screen\n");
	fprintf(fd, "    --only-tokens  Only print tokens\n");
	fprintf(fd, "    --only-ast     Only print ast\n");
	fprintf(fd, "    --only-st      Only print symbol table per scope\n");
	fprintf(fd, "\n");
	fprintf(fd, "MORE INFO:\n");
	fprintf(fd, "    -> To read from stdin run as follows './lemon -'\n");
	fprintf(fd, "\n");
}

