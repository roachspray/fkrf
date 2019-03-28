PACKAGE=krf
FILESDIR=.
SRCS	= krf.c config.c syscalls.c syscalls/chdir.gen.c
KMOD	= krf 
CFLAGS+= -Wno-incompatible-function-pointer-types 

clean:
	rm -rf machine x86 .depend.* *.o *.ko syscalls/*.o export_syms

genclean:
	rm -rf *.gen.x *.gen.h syscalls/*.gen.x syscalls/*.gen.h syscalls/*.gen.c

.include <bsd.kmod.mk>
