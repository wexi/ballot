PROGS := vote quorum

MMM=i686-pc-mingw32.static
MXE=${HOME}/mxe/usr
MGW=${MXE}/${MMM}

_PATH=${MXE}/bin:${MGW}/bin:/usr/local/bin:/usr/bin:/bin
_CPATH=${MXE}/include:${MGW}/include

CFLAGS ?= -Wall -Wextra -g

EXECS := $(PROGS) $(addsuffix .exe,$(PROGS))

all: $(EXECS)

define BUILD
$(1): $(1).c
	gcc $(CFLAGS) -o $(1) $(1).c -lsqlite3 -lreadline
$(1).exe: $(1).c
	PATH=$(_PATH) CPATH=$(_CPATH) $(MMM)-gcc $(CFLAGS) -o $(1).exe $(1).c -lsqlite3 -lreadline -lcurses
endef

$(foreach PROG,$(PROGS),$(eval $(call BUILD,$(PROG))))

clean:
	rm -f $(EXECS)
