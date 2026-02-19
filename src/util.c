#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================================
// util.h - definition
// ========================================

char *read_file(const char *filepath) {
	FILE *fd = stdin;
	if (strcmp(filepath, "-") != 0) fd = fopen(filepath, "r");
	if (fd == NULL) {
		char buffer[1024];
		snprintf(buffer, 1024, "Error opening '%s'", filepath);
		perror(buffer);
		exit(1);
	}

	int cap = 1024, len = 0;
	char *buffer = (char *) malloc(cap * sizeof(char));
	if (buffer == NULL) {
		perror("Error in read_file with malloc");
		exit(1);
	}

	for (;;) {
		int n = fread(buffer, 1, cap - len, fd);
		if (n == 0) {
			break;
		}

		len += n;
		if (len == cap) {
			cap *= 2;
			buffer = realloc(buffer, cap * sizeof(char));
			if (buffer == NULL) {
				perror("Error in read_file with realloc - 1");
				exit(1);
			}
		}
	}

	buffer = realloc(buffer, len + 1);
	if (buffer == NULL) {
		perror("Error in read_file with realloc - 2");
		exit(1);
	}
	buffer[len] = '\0';

	if (strcmp(filepath, "-") != 0) fclose(fd);
	return buffer;
}

