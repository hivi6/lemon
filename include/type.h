#ifndef TYPE_H
#define TYPE_H

enum {
	TY_INT,
};

struct type_t {
	int type;
	int size;
};

typedef struct type_t type_t;

/**
 * Get the int type
 *
 * Returns:
 * 	Get the int type
 */
type_t *type_int();

#endif // TYPE_H

