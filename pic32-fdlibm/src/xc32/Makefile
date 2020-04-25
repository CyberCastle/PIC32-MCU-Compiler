# 
# Makefile for libmfd.a
#
# This makefile will only work with GNU make
#

ROOT := $(shell pwd)

include $(ROOT)/defines.mk

MAKEFILE = -f $(ROOT)/fdlibm.mk

SRCINSTALLDIRS=

all install: install-headers
	@$(MAKE) doall T=$@

install-baseline:
	@$(MAKE) doall T=install

install-proc:

install-src:

clean:
	@$(MAKE) doall T=$@
	[ ! -d OBJ ] || rm -r OBJ
	rm -f *[~#]

doall: dodir
	$(foreach ML,$(shell $(CC) --print-multi-lib), \
		set -e; \
		rdir=`echo "$(ML)" | sed -e 's/;.*$$//'`; \
		dir=OBJ/$$rdir; \
		[ -d $$dir ] || mkdir -p $$dir; \
		flags=`echo "$(ML)" | sed -e 's/^[^;]*;//' -e 's/@/ -/g'`; \
		if echo "$$flags" | egrep -v '$(SKIPARCHS)' >/dev/null; then \
			if echo "$$flags" | grep 'mips16' >/dev/null; then \
				makevar="CODESIZEOPT=1"; \
			else \
				makevar=""; \
			fi; \
			echo $(MAKE) -C $$dir $(MAKEFILE) $$makevar VAR="$$flags" SUBDIR=$$rdir $F $T; \
			$(MAKE) -C $$dir $(MAKEFILE) $$makevar VAR="$$flags" SUBDIR=$$rdir $F $T; \
		fi; ) \

doone: dodir
	set -e; \
	rdir=`$(CC) --print-multi-dir $(VAR)`; \
	dir=OBJ/$$rdir; \
	[ -d $$dir ] || mkdir -p $$dir; \
	$(MAKE) -C $$dir $(MAKEFILE) VAR="$(VAR)" SUBDIR=$$rdir $F $T

dodir: Makefile $(ROOT)/fdlibm.mk
	mkdir -p OBJ

install-headers: 
	mkdir -p $(TGTDESTDIR)/include;
	$(foreach header_file,$(shell find ../ -name "*.h" | grep -v fdlibm.h | grep -v .svn | grep -v xc16 | grep -v xc8), \
		cp $(header_file) ${TGTDESTDIR}/include/$(shell basename $(header_file)) 2>>/dev/null; )


.PHONY: all install clean doall doone dodir install-baseline install-proc
