/* uroot - User's root
 *   sigpipe.c Source file for signaling pipe implementation
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
#include "sigpipe.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

sigpipe_t sigpipe_new() {
	sigpipe_t sp = malloc(2 * sizeof(int));
	if (!pipe(sp))
		assert_perror(errno);
	return sp;
}

void sigpipe_wait(sigpipe_t sp) {
	close(sp[1]);
	char ch;
	while (read(sp[0], &ch, 1) > 0);
	assert_perror(errno);
	free(sp);
}

void sigpipe_signal(sigpipe_t sp) {
	close(sp[0]);
	close(sp[1]);
	free(sp);
}
