#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <err.h>

#include <sys/sysctl.h>

#include "krfsys.h"


int main(int argc, char *argv[]) {
  int syscallno = -1;

  if (argc < 3 || !strcmp(argv[1], "-h")) {
    printf("usage: krfexec <syscall#> <command or file> [args]\n");
    return 1;
  }

  syscallno = atoi(argv[1]);
  if (syscallno < 0) {
    printf("krfexec: invalid system call specified for faultable()\n");
    return 1;
  }

  // XXX: check ret
  syscall(syscallno, KRF_ENABLE);

  struct rlimit core_limit;
  core_limit.rlim_cur = core_limit.rlim_max = RLIM_INFINITY;
  if (setrlimit(RLIMIT_CORE, &core_limit) < 0) {
    err(errno, "setrlimit");
  }

  if (execvp(argv[2], argv + 2) < 0) {
    err(errno, "exec %s", argv[1]);
  }
  return 0;
}
