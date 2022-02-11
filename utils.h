/* uroot - User's root
 *   utils.h Header file with various utility functions
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
#ifndef _UROOT_UTILS_H_
#define _UROOT_UTILS_H_

#define _GNU_SOURCE
#include <alloca.h>
#include <sys/types.h>

// Compute the size needed (including \0) to format given message
size_t printf_len(const char *msg, ...) __attribute__((format(printf, 1, 2)));
// Like sprintf, but returs the string. Expects there's enough space.
char *printf_into(char *dst, const char *msg, ...) __attribute__((format(printf, 2, 3)));
// Like printf, but allocates the data on the stack with alloca and returns. It
// uses the arguments multiple times, so beware of side effects.
#define aprintf(...) printf_into(alloca(printf_len(__VA_ARGS__)), __VA_ARGS__)

// returns path to shell interpreter
const char *get_shell();

enum mapidtype {
	MAP_ID_USER,
	MAP_ID_GROUP
};

// call newuidmap or newgidmap for process of given pid
int new_map_id(enum mapidtype type, pid_t pid, int id);

#endif
