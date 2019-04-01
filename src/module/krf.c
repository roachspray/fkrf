#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysproto.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/sysctl.h>
#include <sys/conf.h>

#include <sys/syscall.h>

#include "config.h"
#include "syscalls.h"


static void krf_flush_table(void);

static struct sysctl_ctx_list clist;
static struct sysctl_oid *krf_sysctl_root;

static unsigned  load_fail = 0;

/*
 * Handle the setting of faultable syscalls and clearing of them.
 */
static int
control_file_sysctl(SYSCTL_HANDLER_ARGS)
{
	int syscall_num = -1;
	int error = 0;

	error = sysctl_handle_int(oidp, &syscall_num, 0, req);
	if (error) {
		printf("krf: sysctl_handle_int() error\n");
		return error;
	}
 	if (!req->newptr) {
		printf("krf: sysctl_handle_int() error: !req->newptr\n");
		return error;
	}

	if (syscall_num > 0 && syscall_num < KRF_MAX_SYSCALL) {
		if (krf_faultable_table[syscall_num].sy_call != NULL) {
			printf("krf: faulting enabled for syscall %d\n",
			    syscall_num);
			// yolo
			sysent[syscall_num].sy_call = \
			    krf_faultable_table[syscall_num].sy_call;
		} else {
			printf("krf: unsupported syscall\n");
			return EOPNOTSUPP;
		}
	} else if (syscall_num > KRF_MAX_SYSCALL) {
		printf("krf: clearing all enabled faults\n");
		krf_flush_table();
	} else {
		printf("krf: unsupported syscall\n");
		return EOPNOTSUPP;
	}
	return 0;

}

static int
krf_init(void)
{
	int  err = 0;

	/* do entire sysent struct even though we don't need it :-/ */
	memset(krf_faultable_table, 0, KRF_NR_SYSCALLS * sizeof(struct sysent));
	memcpy(krf_sys_call_table, sysent, KRF_NR_SYSCALLS * sizeof(struct sysent));

	sysctl_ctx_init(&clist);
	krf_sysctl_root = SYSCTL_ADD_ROOT_NODE(&clist, OID_AUTO,
	    "krf", CTLFLAG_RW, 0, "krf sysctl root node");
	if (krf_sysctl_root == NULL) {
		printf("ERROR: Adding root sysctl node failed.\n");
		return -1;
	}

	SYSCTL_ADD_UINT(&clist, SYSCTL_CHILDREN(krf_sysctl_root),
	    OID_AUTO, "rng_state", CTLFLAG_RW, &krf_rng_state,
	    krf_rng_state, "Set the RNGs state");

	SYSCTL_ADD_UINT(&clist, SYSCTL_CHILDREN(krf_sysctl_root),
	    OID_AUTO, "personality", CTLFLAG_RW, &krf_personality,
	    krf_personality, "Configure the bit to set for enabling krf");

	SYSCTL_ADD_UINT(&clist, SYSCTL_CHILDREN(krf_sysctl_root),
	    OID_AUTO, "probability", CTLFLAG_RW,
	    &krf_probability, krf_probability, "Set the probability of fault");

	SYSCTL_ADD_PROC(&clist, SYSCTL_CHILDREN(krf_sysctl_root),
	    OID_AUTO, "control_file", CTLTYPE_INT | CTLFLAG_RW,
	    &krf_control, krf_control, control_file_sysctl, "I",
	    "Enable different syscalls to fault with this");

	SYSCTL_ADD_UINT(&clist, SYSCTL_CHILDREN(krf_sysctl_root),
	    OID_AUTO, "log_faults_file",  CTLFLAG_RW,
	    &krf_log_faults, krf_log_faults, 
	    "Enable/disable logging faults to syslog");


	return err;
}

/** XXX **/
static void
krf_flush_table(void)
{
	int nr;

	/* XXX: Eh, I probably need a lock here. */	
	for (nr = 0; nr < KRF_MAX_SYSCALL; nr++) {
		if (krf_sys_call_table[nr].sy_call) {
			 sysent[nr].sy_call = krf_sys_call_table[nr].sy_call;
		}
	}
}

static void
krf_teardown(void)
{
	krf_flush_table();
	if (load_fail == 0) {
		sysctl_remove_oid(krf_sysctl_root, 1, 0);
	}
	sysctl_ctx_free(&clist);
}

static int
kldload_entry(module_t mod, int cmd, void *arg)
{
        int error;

        error = 0;
        switch (cmd) {
        case MOD_LOAD:
		error = krf_init();
		if (error != 0) {
			printf("krf: Failed to load.\n");
			load_fail = 1;
		}

#include "krf.gen.x"

		printf("krf: loaded\n");
                break;
        case MOD_UNLOAD:
		krf_teardown();
		printf("krf: unloaded.");
                break;
        default:
                break;
        }
        return (error);
}

static moduledata_t mod_data = {
	"krf",
	kldload_entry,
	0
};
 
DECLARE_MODULE(krf, mod_data, SI_SUB_EXEC, SI_ORDER_ANY);
