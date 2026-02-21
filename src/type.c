#include "type.h"

#include <stdio.h>

// ========================================
// helper declaration
// ========================================

static type_t *g_int = &(type_t) {TY_INT, 4};

// ========================================
// type.h - definition
// ========================================

type_t *type_int() {
	return g_int;
}

void print_all_types() {
	printf("id: %p | name: %s\n", g_int, "int");
}

