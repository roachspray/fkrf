#
# Im sorry, but I have butchered this makefile bc I seriously suck at them.
#



all:
	ruby gentable
	ruby genprofiles
	$(CC) -o krfctl krfctl.c table.gen.c profiles.gen.c

clean:
	rm -f krfctl
	rm -f *.gen.c
