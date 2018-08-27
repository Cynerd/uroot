#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sched.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include "utils.h"

void sigint_handler(int sig) { }

int main(int argc, char **argv) {
	pid_t ppid = getpid();

	if (!fork()) {
		system(aprintf("newuidmap %d 0 %d 1 1 65537 65536", ppid, getuid()));
		system(aprintf("newgidmap %d 0 %d 1 1 65537 65536", ppid, getgid()));
		kill(ppid, SIGINT);
		return 0;
	}

	unshare(CLONE_NEWNS | CLONE_NEWUSER | CLONE_NEWPID);

	signal(SIGINT, sigint_handler);
	pause();
	errno = 0; // Just clear error from pause()

	pid_t chpid = fork();
	if (chpid) {
		int stat;
		waitpid(chpid, &stat, 0);
		return stat;
	}

	// mount /sys and /proc
	mount("none", "/", NULL, MS_REC | MS_PRIVATE, NULL);
	assert_perror(errno);
	mount("none", "/proc", NULL, MS_REC | MS_PRIVATE, NULL);
	assert_perror(errno);
	mount("none", "/sys", NULL, MS_REC | MS_PRIVATE, NULL);
	assert_perror(errno);
	mount("proc", "/proc", "proc", MS_NOSUID|MS_NODEV|MS_NOEXEC, NULL);
	assert_perror(errno);

	// mount proc to root
	// rbind mount dev and sys to root

	// TODO verify that all upper directories have +rx rights
	// TODO chroot
	execv("/bin/sh", NULL);
}
