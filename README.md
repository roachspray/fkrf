# fkrf: FreeBSD 12.0 port of KRF

This is a (hacked up) port of [Trail of Bits'](https://www.trailofbits.com) 
[krf](https://github.com/trailofbits/krf) utility from Linux to FreeBSD 12.


All credit goes to ToB and [William Woodruff](https://github.com/woodruffw) for
the design and development of the linux krf tool and for much of the code here
in this port. Much of what I did was just copy/paste and then hack, I mean, fix
it to work in FreeBSD.

I highly recommend reading their github materials prior to using this.

**Currently** as of the initial commit, this is incomplete in the number of 
syscalls supported. Numerous of the YAML specs are filled out, but no where
complete. Further, I have removed numerous specs I was not planning to 
support...most of these are in the realm of security features such as MAC and
Capabilities.


## Some Differences

Since it is for FreeBSD and this is kernel based there are clearly some
implementation differences. I did not note what I changed in my hackfest
that would differ from their implementation but I will try to list them 
below:


- Implement the configuration settings using sysctl instead of procfs
- Using __typeof() instead of typeof() (which I should have just #define'd)
- You'll recall how arguments are in FreeBSD... that is a $syscall_args struct.
- Copy full sysent table (memory hog :( this can be cleaned up)
- Since no personality(2) in FreeBSD and procctl(2) is not easily extended from
a KLD, I chose to use some free space in the p_flag2 element of struct proc. This
allows for enable/disable of KRF for each process. To do this, I (stupidly?) add
a system call for enable/disable/what is the status .. it is krfsys. Note I have
to hook fork-like calls to ensure the child procs are KRF'd. Passing onto child
could be optional, but I have not made it that way. krfsys should be loaded before
krf.ko because of the hooking of fork calls.
- I probably should have locking or something around sysent entry changes :-o
- Probably other things..will add as I remember.


## Setup for use

To test this setup (yes, please ignore my heavy handed
root use here :P):

```
# cd src/krfsys
# make
... must be loaded before krf.ko
# kldload ./krfsys.ko
# dmesg .... and look for the syscall number (yes, remember, hack! :D) should be 210
# cd ../module/codegen
# ruby codegen
# cd ..
# make
# kldload ./krf.ko
# cd ../krfctl
# make
# ./krfctl -F chdir
# cd ../krfexec
# make
# cd ../../examples
# clang -o rep_chdir rep_chdir.c
# ../src/krfexec/krfexec <syscalloffaultable> ./rep_chdir
```


## System calls hooked

Note that some of the available system calls are not spec'd to be hooked. 
This is partly on purpose and partly because things are not complete as yet.
