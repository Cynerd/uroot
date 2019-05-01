/* uroot - User's root
 *   child.c Source file for child with unshared resources
 *
 * Copyright (C) 2018-2019 Karel Kočí
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

#define PROC_BINFMT "/proc/sys/fs/binfmt_misc"

int child_main(void *_args) {
	struct child_args *args = _args;
	sigpipe_wait(args->sigpipe);

	// Change all mount points to private
	mount("none", "/", NULL, MS_REC | MS_PRIVATE, NULL);
	assert_perror(errno);

	// Mount new proc filesystem for this namespace
	if (args->binfmt) {
		mount(PROC_BINFMT, "/tmp", NULL, MS_BIND, NULL); 
		assert_perror(errno);
	}
	mount("proc", "/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC, NULL);
	assert_perror(errno);
	if (args->binfmt) {
		mount("/tmp", PROC_BINFMT, NULL, MS_MOVE, NULL); 
		assert_perror(errno);
	}

	if (args->argc > 0) {
		execvp(args->argv[0], args->argv);
		assert_perror(errno);
	} else {
		const char *shell = get_shell();
		execl(shell, shell, NULL);
	}
	return 1;
}
