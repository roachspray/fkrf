#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <sys/sysctl.h>

#include "krfctl.h"

/* control will interpret any number larger than its syscall table
 * as a command to clear all current masks.
 * it's a good bet that linux^WFreeBSD will never have 65535 syscalls.
 */
#define CLEAR_MAGIC 65535

static int lookup_syscall_number(const char *sys_name) {
  for (syscall_lookup_t *elem = syscall_lookup_table; elem->sys_name != NULL; elem++) {
    if (!strcmp(sys_name, elem->sys_name)) {
      return elem->sys_num;
    }
  }
  return -1;
}

static const char **lookup_syscall_profile(const char *profile) {
  for (fault_profile_t *elem = fault_profile_table; elem->profile != NULL; elem++) {
    if (!strcmp(profile, elem->profile)) {
      return elem->syscalls;
    }
  }

  return NULL;
}

static void fault_syscall(const char *sys_name) {
  int sys_num;
  int error = 0;

  sys_num = lookup_syscall_number(sys_name);
  if (sys_num == -1) {
    errx(1, "couldn't find syscall: %s", sys_name);
  }

  error = sysctlbyname("krf.control_file", NULL, NULL, &sys_num, sizeof(int));
  if (error == -1) {
    err(errno, "sysctlbyname");
  }

}

static void fault_syscall_spec(const char *s) {
  const char *sys_name = NULL;

  char *spec = strdup(s);

  sys_name = strtok(spec, ", ");
  while (sys_name) {
    fault_syscall(sys_name);
    sys_name = strtok(NULL, ", ");
  }

  free(spec);
}

static void fault_syscall_profile(const char *profile) {
  const char **syscalls = lookup_syscall_profile(profile);

  if (syscalls == NULL) {
    errx(1, "couldn't find fault profile: %s", profile);
  }

  int i;
  for (i = 0; syscalls[i]; i++) {
    fault_syscall(syscalls[i]);
  }
}

static void clear_faulty_calls(void) {
  int error = 0;
  int clm = CLEAR_MAGIC;

  error = sysctlbyname("krf.control_file", NULL, NULL, &clm, sizeof(int));
  if (error == -1) {
    err(errno, "sysctlbyname");
  }
}

static void set_rng_state(const char *state) {
  int error;
  int rng_state = 0;

  rng_state = strtol(state, NULL, 0);
  error = sysctlbyname("krf.rng_state", NULL, NULL, &rng_state, sizeof(int));
  if (error == -1) {
    err(errno, "sysctlbyname: krf.probability");
  }

}

static void set_prob_state(const char *state) {
  int error = 0;
  int prob = 0;

  prob = strtol(state, NULL, 0);

  error = sysctlbyname("krf.probability", NULL, NULL, &prob, sizeof(int));
  if (error == -1) {
    err(errno, "sysctlbyname: krf.probability");
  }
}

static void toggle_fault_logging(void) {
  int loggle = 0;
  size_t len = 0;
  int error = 0;

  error = sysctlbyname("krf.log_faults_file", &loggle, &len, NULL, 0);
  if (error == -1) {
    err(errno, "sysctlbyname: krf.log_faults_file: read");
  }
  
  loggle = !loggle;
  error = sysctlbyname("krf.log_faults_file", NULL, NULL, &loggle, sizeof(int));
  if (error == -1) {
    err(errno, "sysctlbyname: krf.log_faults_file: write");
  }
}

int main(int argc, char *argv[]) {

  int c;
  while ((c = getopt(argc, argv, "F:P:cr:p:L")) != -1) {
    switch (c) {
    case 'F': {
      fault_syscall_spec(optarg);
      break;
    }
    case 'P': {
      fault_syscall_profile(optarg);
      break;
    }
    case 'c': {
      clear_faulty_calls();
      break;
    }
    case 'r': {
      set_rng_state(optarg);
      break;
    }
    case 'p': {
      set_prob_state(optarg);
      break;
    }
    case 'L': {
      toggle_fault_logging();
      break;
    }
    default: {
      printf("usage: krfctl <options>\n"
             "options:\n"
             " -h                          display this help message\n"
             " -F <syscall> [syscall...]   fault the given syscalls\n"
             " -P <profile>                fault the given syscall profile\n"
             " -c                          clear the syscall table of faulty calls\n"
             " -r <state>                  set the RNG state\n"
             " -p <prob>                   set the fault probability\n"
             " -L                          toggle faulty call logging\n");
      return 1;
    }
    }
  }

  return 0;
}
