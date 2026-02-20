#include "analyze.h"
#include "error.h"

// ========================================
// helper declaration
// ========================================

void analyze_prog(ast_t *ast);
void analyze_stmt(ast_t *ast);

// ========================================
// analyzer.h - definition
// ========================================

void analyze(ast_t *ast) {
	analyze_prog(ast);
}

// ========================================
// helper definition
// ========================================

void analyze_prog(ast_t *ast) {
	for (ast_t *cur = ast; cur; cur = cur->next) {
		analyze_stmt(ast);
	}
}

void analyze_stmt(ast_t *ast) {
	switch (ast->type) {
	}
}

