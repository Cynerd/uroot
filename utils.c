#include "utils.h"

// Compute the size needed (including \0) to format given message
size_t printf_len(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	size_t result = vsnprintf(NULL, 0, msg, args);
	va_end(args);
	return result + 1;
}

// Like sprintf, but returs the string. Expects there's enough space.
char *printf_into(char *dst, const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	vsprintf(dst, msg, args);
	va_end(args);
	return dst;
}
