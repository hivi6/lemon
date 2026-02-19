#ifndef UTIL_H
#define UTIL_H

/**
 * Read the content of a file; if filepath == '-' then read from stdin
 *
 * Params:
 * 	filepath  File whose contents needs to be read
 *
 * Returns:
 * 	character array containing the source (Users responsibility of memory)
 */
char *read_file(const char *filepath);

#endif // UTIL_H

