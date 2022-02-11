/* uroot - User's root
 *   main.c Source file with main function
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
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <argp.h>
#include <string.h>
#include "child.h"
#include "utils.h"
#include "sigpipe.h"
#include "conf.h"

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
static const char usage_doc[] = "[CMD [ARG..]]";
static const char doc[] = "Tool using Linux namespaces to provide root like functionality for unprivileged users.";

struct opts {
	char **child_argv;
	size_t child_argc;
	bool binfmt;
};

enum option_key {
	OPT_BINFMT = 260,
};

static struct argp_option options[] = {
	{"debug", 'd', NULL, 0, "Report uroot operations.", 0},
	{"binfmt", OPT_BINFMT, NULL, 0, "Preserve registered misc binary format handlers.", 0},
	{NULL}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct opts *opts = state->input;
	switch (key) {
		case 'd':
			// TODO logging
			break;
		case OPT_BINFMT:
			opts->binfmt = true;
			break;
		case ARGP_KEY_ARGS:
			opts->child_argc = state->argc - state->next;
			opts->child_argv = malloc((opts->child_argc + 1) * sizeof *opts->child_argv);
			memcpy(opts->child_argv, state->argv + state->next, opts->child_argc *
					sizeof *opts->child_argv);
			opts->child_argv[opts->child_argc] = NULL;
			break;
		case ARGP_KEY_INIT:
			opts->child_argv = NULL;
			opts->child_argc = 0;
			opts->binfmt = false;
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	};
	return 0;
}

static const struct argp argp_parser = {
	.options = options,
	.parser = parse_opt,
	.args_doc = usage_doc,
	.doc = doc,
};


int main(int argc, char **argv) {
	struct opts opts;
	argp_parse(&argp_parser, argc, argv, ARGP_IN_ORDER, 0, &opts);

	struct child_args chargs = (struct child_args) {
		.ppid = getpid(),
		.argc = opts.child_argc,
		.argv = opts.child_argv,
		.sigpipe = sigpipe_new(),
		.binfmt = opts.binfmt,
	};

	uint8_t stack[STACK_SIZE];
	pid_t chpid = clone(child_main, stack + STACK_SIZE,
			SIGCHLD | CLONE_NEWNS | CLONE_NEWUSER | CLONE_NEWPID,
			&chargs);

#define FAIL(MSG) do { fputs(MSG, stderr); kill(chpid, SIGKILL); return 1; } while (false);
	if (new_map_id(MAP_ID_USER, chpid, getuid()))
		FAIL("Mapping of uid failed!\n");
	if (new_map_id(MAP_ID_GROUP, chpid, getgid()))
		FAIL("Mapping of gid failed!\n");

	sigpipe_signal(chargs.sigpipe);

	int stat;
	waitpid(chpid, &stat, 0);
	if (WIFEXITED(stat))
		return WEXITSTATUS(stat);
	else
		return WIFSIGNALED(stat) || WCOREDUMP(stat);
}
