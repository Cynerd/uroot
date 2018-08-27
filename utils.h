#ifndef _UTILS_H_
#define _UTILS_H_

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <alloca.h>

// Compute the size needed (including \0) to format given message
size_t printf_len(const char *msg, ...) __attribute__((format(printf, 1, 2)));
// Like sprintf, but returs the string. Expects there's enough space.
char *printf_into(char *dst, const char *msg, ...) __attribute__((format(printf, 2, 3)));
// Like printf, but allocates the data on the stack with alloca and returns. It
// uses the arguments multiple times, so beware of side effects.
#define aprintf(...) printf_into(alloca(printf_len(__VA_ARGS__)), __VA_ARGS__)

#endif /* _UTILS_H_ */
