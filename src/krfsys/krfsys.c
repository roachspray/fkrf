/*
 * This should be moved to the krf KLD instead of as a separate
 * KLD... mostly I forgot proper way to do things and so was
 * just wanting to get the syscall added and working.
 *
 */
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysproto.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/syscall.h>

#include "krfsys.h"

/*
 * faultable() is the syscall we add to enable KRF,
 * disable KRF, and check status of enable/disable
 * for a given process.
 *
 */
static int
faultable(struct thread *td, void *arg)
{
	struct proc *p;
	int error = 0;

	struct faultable_args *uap = (struct faultable_args *)arg;
	if (uap->faultable == KRF_ENABLE) {
                p = td->td_proc;
		PROC_LOCK(p);
		p->p_flag2 |= KRF_FAULTABLE_FLAG;
		PROC_UNLOCK(p);
	} if (uap->faultable == KRF_DISABLE) {
                p = td->td_proc;
		PROC_LOCK(p);
		p->p_flag2 &= ~KRF_FAULTABLE_FLAG;
		PROC_UNLOCK(p);
	} else {
                p = td->td_proc;
		PROC_LOCK(p);
		error = (p->p_flag2 & KRF_FAULTABLE_FLAG);
		PROC_UNLOCK(p);
	}
		
	return (error);
}

/*
 * Since we introduce bits to p_flag2, we should make sure
 * that propagation of the KRF enable/disable bit occurs to
 * the spawned child process. Do this by intercepting fork-
 * like calls.
 *
 */
static int
hack_fork(struct thread *td, void *arg)
{
	int err = 0;
	struct fork_args *uap = (struct fork_args *)arg;

	err = sys_fork(td, uap);
	if (err == 0) {
		struct proc *p = pfind(td->td_retval[0]);
		p->p_flag2 |= (td->td_proc->p_flag2 & KRF_FAULTABLE_FLAG);
		PROC_UNLOCK(p);
	}
	return (err);
}
static int
hack_rfork(struct thread *td, void *arg)
{
	int err = 0;
	struct rfork_args *uap = (struct rfork_args *)arg;

	err = sys_rfork(td, uap);
	if (err == 0) {
		struct proc *p = pfind(td->td_retval[0]);
		p->p_flag2 |= (td->td_proc->p_flag2 & KRF_FAULTABLE_FLAG);
		PROC_UNLOCK(p);
	}
	return (err);
}

static int
hack_vfork(struct thread *td, void *arg)
{
	int err = 0;
	struct vfork_args *uap = (struct vfork_args *)arg;

	err = sys_vfork(td, uap);
	if (err == 0) {
		struct proc *p = pfind(td->td_retval[0]);
		p->p_flag2 |= (td->td_proc->p_flag2 & KRF_FAULTABLE_FLAG);
		PROC_UNLOCK(p);
	}
	return (err);
}

static int
hack_pdfork(struct thread *td, void *arg)
{
	int err = 0;
	struct pdfork_args *uap = (struct pdfork_args *)arg;

	err = sys_pdfork(td, uap);
	if (err == 0) {
		struct proc *p = pfind(td->td_retval[0]);
		p->p_flag2 |= (td->td_proc->p_flag2 & KRF_FAULTABLE_FLAG);
		PROC_UNLOCK(p);
	}
	return (err);
}

static struct sysent faultable_sysent = {
	1,
	faultable
};

static int offset = NO_SYSCALL;

static int
load(struct module *module, int cmd, void *arg)
{
	int error = 0;

	switch (cmd) {
	case MOD_LOAD :
		printf("krfsys: faultable() loaded at: %d\n", offset);
		sysent[SYS_fork].sy_call = &hack_fork;
		sysent[SYS_rfork].sy_call = &hack_rfork;
		sysent[SYS_vfork].sy_call = &hack_vfork;
		sysent[SYS_pdfork].sy_call = &hack_pdfork;
		break;
	case MOD_UNLOAD :
		printf("krfsys: fultable() unloaded\n");
		sysent[SYS_fork].sy_call = (sy_call_t *)&sys_fork;
		sysent[SYS_rfork].sy_call = (sy_call_t *)&sys_rfork;
		sysent[SYS_vfork].sy_call = (sy_call_t *)&sys_vfork;
		sysent[SYS_pdfork].sy_call = (sy_call_t *)&sys_pdfork;
		break;
	default :
		error = EOPNOTSUPP;
		break;
	}
	return (error);
}

SYSCALL_MODULE(krfsys, &offset, &faultable_sysent, load, NULL);
