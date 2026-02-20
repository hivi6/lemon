#include "type.h"

// ========================================
// helper declaration
// ========================================

static type_t *g_int = &(type_t) {TY_INT, 1};

// ========================================
// type.h - definition
// ========================================

type_t *type_int() {
	return g_int;
}

