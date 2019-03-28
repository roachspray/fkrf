# fkrf: FreeBSD 12.0 port of KRF

This is a (lame) port of [Trail of Bits'](https://www.trailofbits.com) 
[krf](https://github.com/trailofbits/krf) utility.

# NOTE:

All credit goes to ToB and [William Woodruff](https://github.com/woodruffw) for
the design and development of the linux krf tool and for much of the code here
in this port. Much of what I did was just copy/paste and then hack, I mean, fix
it to work in FreeBSD.

**Currently** as of the initial commit, this is incomplete in the number of 
syscalls supported. In fact, just one is supported. Hopefully by tomorrow I 
will write a script to add to the set of YAML files specifying the syscalls.

...

This will be filled out hopefully tomorrow.

To test this single syscall setup now (yes, please ignore my heavy handed
root use here :P):

```
# cd src/krfsys
# make
# kldload ./krfsys.ko
# dmesg .... and look for the syscall number (yes, remember, hack! :D) should be 210
# cd ../module/codegen
# ruby codegen
# cd ..
# make
# kldload ./krf.ko
# cd ../krfctl
# make
# ./krfctl chdir
# cd ../krfexec
# make
# cd ../../examples
# clang -o rep_chdir rep_chdir.c
# ../src/krfexec/krfexec <syscallfromabove> ./rep_chdir
```

