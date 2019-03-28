#ifndef	__KRFSYS_H
#define	__KRFSYS_H

#define	KRF_ENABLE	1
#define	KRF_DISABLE	2

// Similar to KRF for Linux.. this is an unused flag, beware tho
#define	KRF_FAULTABLE_FLAG	0x01000000

struct faultable_args {
	unsigned int faultable;	// 0 == no, not 0 == yes
};
#endif
