/* uroot - User's root
 *   child.c Source file for child with unshared resources
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#define _GNU_SOURCE
#include "child.h"
#include <stdlib.h>
#include <sys/mount.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "utils.h"

int child_main(void *_args) {
	struct child_args *args = _args;
	sigpipe_wait(args->sigpipe);

	// Change some mount points to private
	mount("none", "/", NULL, MS_REC | MS_PRIVATE, NULL);
	assert_perror(errno);
	/*
	mount("none", "/proc", NULL, MS_REC | MS_PRIVATE, NULL);
	assert_perror(errno);
	mount("none", "/sys", NULL, MS_REC | MS_PRIVATE, NULL);
	assert_perror(errno);
	*/
	// Mount new proc filesystem for this namespace
	mount("proc", "/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC, NULL);
	assert_perror(errno);
	//mount("binfmt_misc", "/proc/sys/fs/binfmt_misc", "binfmt_misc", MS_NOSUID|MS_NODEV|MS_NOEXEC, NULL);
	//assert_perror(errno);

	if (args->argc <= 1) {
		const char *shell = get_shell();
		execl(shell, shell, NULL);
	} else {
		char *new_argv[args->argc + 1];
		memcpy(new_argv, args->argv + 1, args->argc * sizeof *new_argv);
		new_argv[args->argc] = NULL;
		execvp(args->argv[1], new_argv);
		assert_perror(errno);
	}
	return 1;
}
