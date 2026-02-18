#include <stdio.h>

#include "error.h"

int main() {
	const char *filepath = "stdin";
	const char *src = "abcd\n\thello\t123\n456";

	pos_t start = POS_INIT;
	pos_t end = (pos_t) {.index=6, .line=3, .column=1};

	error_print(filepath, src, start, end, "some error");

	return 0;
}

