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
#include <string.h>
#include <pwd.h>
#include <grp.h>
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

struct subid {
	char *start;
	char *count;
};

struct subid parse_sub(const char *idtp, const char *name) {
	FILE *f = fopen(aprintf("/etc/sub%s", idtp), "r");
	struct subid res = {NULL, NULL};
	char *line = NULL;
	ssize_t len = 0;
	while ((len = getline(&line, &len, f)) != -1) {
		char *firstcol = strchr(line, ':');
		if (firstcol == NULL || strncmp(name, line, firstcol - line))
			continue;
		char *seccol = strchr(firstcol + 1, ':');
		if (seccol == NULL)
			continue;
		res.start = strndup(firstcol + 1, (seccol - firstcol) - 1);
		res.count = strndup(seccol + 1, len - 1 - (seccol - line) - (line[len - 1] == '\n' ? 1 : 0));
	}
	free(line);
	fclose(f);
	return res;
}

int new_map_id(enum mapidtype type, pid_t pid, int id) {
	// TODO possibly we should use all maps provided by sub* files for given
	// account. It is common that there is only one mapping but there can
	// technically be multiple ones.
	char const *idtp;
	struct subid subid;
	switch (type) {
		case MAP_ID_USER:
			idtp = "uid";
			struct passwd *upass = getpwuid(getuid());
			subid = parse_sub(idtp, upass->pw_name);
			break;
		case MAP_ID_GROUP:
			idtp = "gid";
			struct group *gpass = getgrgid(getgid());
			subid = parse_sub(idtp, gpass->gr_name);
			break;
	}
	pid_t chld = fork();
	if (!chld) {
		char *tool = aprintf("new%smap", idtp);
		char *const args[] = {
			tool,
			aprintf("%d", pid),
			"0", aprintf("%d", id), "1",
			subid.start && subid.count ? "1" : NULL, subid.start, subid.count,
			NULL
		};
		execvp(tool, args);
		abort();
	}
	int stat;
	waitpid(chld, &stat, 0);
	return stat;
}

