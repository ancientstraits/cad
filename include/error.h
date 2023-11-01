#ifndef ERR_H
#define ERR_H

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(cond, ...) if (!(cond)) DIE(__VA_ARGS__)

#define LOG(...) do { \
	fprintf(stderr, "log %s:%d (%s()): ", __FILE__, __LINE__, __func__); \
	fprintf(stderr, __VA_ARGS__); \
	fputc('\n', stderr); \
} while (0)

#define DIE(...) do { \
	LOG(__VA_ARGS__); \
	abort(); \
} while (0)

#endif // !ERR_H

