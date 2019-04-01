#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysproto.h>
#include <sys/syscall.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include "syscalls.h"
#include "syscalls/internal.h"

/*
 * Probably should just do arrays of sy_call_t for space reasons.
 */
struct sysent krf_faultable_table[KRF_MAX_SYSCALL] = {};
struct sysent krf_sys_call_table[KRF_MAX_SYSCALL] = {};

#include "syscalls.gen.x"
