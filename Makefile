# one or more programs:
PROG := vote
# SQLite database name:
DBNAME := condo

WPROG := $(addsuffix .exe,$(PROG))
ZIP := releases/Windows-binary-release.zip

all: $(PROG)
zip: $(ZIP)

MMM=i686-w64-mingw32.static
MXE=${HOME}/mxe/usr
MGW=${MXE}/${MMM}

_PATH=${MXE}/bin:${MGW}/bin:/usr/local/bin:/usr/bin:/bin
_CPATH=${MXE}/include:${MGW}/include
_CFLAGS ?= -Wall -Wextra -Os -s

CFLAGS ?= -Wall -Wextra -g

define BUILD
$(1): $(1).c
	gcc $(CFLAGS) -D DBNAME=\"$(DBNAME)\" -o $(1) $(1).c -lsqlite3
$(1).exe: $(1).c
	PATH=$(_PATH) CPATH=$(_CPATH) $(MMM)-gcc $(_CFLAGS) -D DBNAME=\"$(DBNAME)\" -o $(1).exe $(1).c -lsqlite3
endef

$(foreach EXE,$(PROG),$(eval $(call BUILD,$(EXE))))

$(ZIP): $(WPROG) $(DBNAME).db
	zip $@ $^

clean:
	rm -f $(PROG) $(WPROG)

