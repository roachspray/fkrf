PROG := krfexec

all: $(PROG)

$(PROG): $(PROG).o

CFLAGS+=-I../krfsys

.PHONY: clean
clean:
	rm -f $(PROG) $(PROG).o
