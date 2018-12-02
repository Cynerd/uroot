/* uroot - User's root
 *   main.c Source file with main function
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
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include "child.h"
#include "utils.h"
#include "sigpipe.h"
#include "conf.h"


int main(int argc, char **argv) {
	// TODO arguments parsing

	struct child_args chargs = (struct child_args) {
		.ppid = getpid(),
		.argc = argc,
		.argv = argv,
		.sigpipe = sigpipe_new()
	};

	uint8_t stack[STACK_SIZE];
	pid_t chpid = clone(child_main, stack + STACK_SIZE,
			SIGCHLD | CLONE_NEWNS | CLONE_NEWUSER | CLONE_NEWPID,
			&chargs);

#define FAIL(MSG) do { fputs(MSG, stderr); kill(chpid, SIGKILL); return 1; } while (false);
	if (new_map_id("uid", chpid, getuid()))
		FAIL("Mapping of uid failed!\n");
	if (new_map_id("gid", chpid, getgid()))
		FAIL("Mapping of gid failed!\n");

	sigpipe_signal(chargs.sigpipe);

	int stat;
	waitpid(chpid, &stat, 0);
	if (WIFEXITED(stat))
		return WEXITSTATUS(stat);
	else
		return WIFSIGNALED(stat) || WCOREDUMP(stat);
}
