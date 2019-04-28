/* uroot - User's root
 *   child.h Header file for child with unshared resources
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
#ifndef _UROOT_CHILD_H_
#define _UROOT_CHILD_H_

#include <stdbool.h>
#include <sys/types.h>
#include "sigpipe.h"

// Arguments passed to child_main function
struct child_args {
	pid_t ppid; // Paret pid
	int argc;
	char **argv;
	sigpipe_t sigpipe;
	bool binfmt;
};

// Function used as a main for child process
int child_main(void *_args);

#endif
