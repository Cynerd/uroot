/* uroot - User's root
 *   utils.c Source file with various utility functions
 *
 * Copyright (C) 2018 Karel Kočí
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"
#include "conf.h"

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

const char *get_shell() {
	const char *ret;
	ret = secure_getenv("SHELL");
	if (!ret)
		ret = DEFAULT_SHELL;
	return ret;
}

int new_map_id(const char *idtp, pid_t pid, int id) {
	// TODO check /etc/sub* for current user subids and map them
	pid_t chld = fork();
	if (!chld) {
		char *tool = aprintf("new%smap", idtp);
		char *const args[] = {
			tool, aprintf("%d", pid),
			"0", aprintf("%d", id), "1",
			"1", "65537", "65536",
			NULL
		};
		execvp(tool, args);
		abort();
	}
	int stat;
	waitpid(chld, &stat, 0);
	return stat;
}

