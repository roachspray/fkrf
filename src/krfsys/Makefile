#	$FreeBSD: releng/12.0/share/examples/kld/dyn_sysctl/Makefile 299293 2016-05-10 00:51:50Z gjb $

PACKAGE=krfsys
FILESDIR=.
SRCS	= krfsys.c
KMOD	= krfsys

clean:
	rm -f export_syms machine x86 krfsys.ko krfsys.kld krfsys.o .depend.*

.include <bsd.kmod.mk>
